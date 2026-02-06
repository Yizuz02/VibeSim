#include "Population.hpp"
#include <random>

// ---------------- CONSTRUCTOR ----------------
Population::Population(Space& space, Theme& theme, float radius)
    : nextId(0),
      radius(radius),
      space(&space),
      theme(&theme)
{
}

// ---------------- CREAR INDIVIDUO ----------------
Individual& Population::createIndividual(std::pair<int,int> pos) {
    long id = nextId++;

    auto individual = std::make_unique<Individual>(
        id,
        radius,      // 🔹 radio viene de la población
        pos,
        *space,
        *theme
    );

    Individual& ref = *individual;
    individuals.emplace(id, std::move(individual));
    return ref;
}

Individual& Population::createIndividual(std::pair<int,int> center, int radius) {
    long id = nextId++;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disty(
        space->minY() + this->radius,
        space->maxY() - this->radius * 2
    );
    std::uniform_int_distribution<int> distx(
        space->minX() + this->radius,
        space->maxX() - this->radius * 2
    );

    Collisions& collisions = space->getCollisions();

    constexpr int MAX_ATTEMPTS = 100;
    std::pair<int,int> pos;
    sf::FloatRect box;
    bool valid = false;

    int squaredRadius = radius * radius;

    for (int i = 0; i < MAX_ATTEMPTS; ++i) {
        pos = { distx(gen), disty(gen) };

        box = sf::FloatRect(
            static_cast<float>(pos.first),
            static_cast<float>(pos.second),
            this->radius * 2.f,
            this->radius * 2.f
        );

        int left   = std::floor(box.left);
        int right  = std::floor(box.left + box.width  - 1);
        int top    = std::floor(box.top);
        int bottom = std::floor(box.top  + box.height - 1);

        

        // --- LÍMITES ---
        auto insideCircle = [&](float x, float y) {
            float dx = x - center.first;
            float dy = y - center.second;
            return (dx * dx + dy * dy) <= radius * radius;
        };

        if (!insideCircle(left,  top))    continue;
        if (!insideCircle(right, top))    continue;
        if (!insideCircle(left,  bottom)) continue;
        if (!insideCircle(right, bottom)) continue;

        if (left < space->minX() || top < space->minY()) continue;
        if (right > space->maxX() || bottom > space->maxY()) continue;

        // --- COLISIONES (MISMA LÓGICA QUE move) ---
        if (collisions.checkVertical(left, right, top)) continue;
        if (collisions.checkVertical(left, right, bottom)) continue;
        if (collisions.checkHorizontal(top, bottom, left)) continue;
        if (collisions.checkHorizontal(top, bottom, right)) continue;

        valid = true;
        break;
    }

    if (!valid) {
        throw std::runtime_error(
            "Population::createIndividual -> No se encontró espacio libre"
        );
    }

    auto individual = std::make_unique<Individual>(
        id,
        this->radius,
        pos,
        *space,
        *theme
    );

    // Registrar colisiones SOLO cuando ya es válido
    collisions.addBox(box);

    Individual& ref = *individual;
    individuals.emplace(id, std::move(individual));
    return ref;
}

Individual& Population::createIndividual() {
    std::pair<int, int> center = {space->minX() + space->getSize().first/2, space->minY() + space->getSize().second/2};
    int radius = (space->getSize().first > space->getSize().second) ? space->getSize().second/2 : space->getSize().first/2;
    return createIndividual(center, radius);
}


// ---------------- AGREGAR INDIVIDUO ----------------
void Population::addIndividual(std::unique_ptr<Individual> individual) {
    if (!individual) return;
    individuals.emplace(individual->getId(), std::move(individual));
}

// ---------------- ELIMINAR UNO ----------------
bool Population::removeIndividual(long id) {
    auto it = individuals.find(id);
    if (it == individuals.end())
        return false;

    // 🔹 Eliminar colisiones del individuo
    space->getCollisions().removeBox(
        it->second->getShape().getGlobalBounds()
    );

    // 🔹 Eliminar individuo
    individuals.erase(it);
    return true;
}

// ---------------- ELIMINAR TODOS ----------------
void Population::clear() {
    // 🔹 Eliminar colisiones de todos los individuos
    for (auto& [id, ind] : individuals) {
        space->getCollisions().removeBox(
            ind->getShape().getGlobalBounds()
        );
    }

    // 🔹 Limpiar población
    individuals.clear();
}

// ---------------- RENDER ----------------
void Population::draw() {
    for (auto& [id, individual] : individuals) {
        individual->draw();
    }
}

// ---------------- GETTERS / SETTERS ----------------
float Population::getRadius() const {
    return radius;
}

void Population::setRadius(float newRadius) {
    radius = newRadius;
}

void Population::setTheme(Theme &theme){
    this->theme = &theme;
    for (auto& [id, individual]: individuals){
        individual->setTheme(&theme);
    }
}

// ---------------- INFO ----------------
size_t Population::size() const {
    return individuals.size();
}

bool Population::empty() const {
    return individuals.empty();
}


long Population::getIndividualAt(float x, float y) const {
    for (const auto& [id, individual] : individuals) {
        if (individual->getShape().getGlobalBounds().contains(x, y)) {
            return id;
        }
    }
    return -1;
}

Individual* Population::getIndividual(long id) {
    auto it = individuals.find(id);
    if (it != individuals.end())
        return it->second.get();
    return nullptr;
}

const Individual* Population::getIndividual(long id) const {
    auto it = individuals.find(id);
    if (it != individuals.end())
        return it->second.get();
    return nullptr;
}

std::unordered_map<long, std::unique_ptr<Individual>>&
Population::getIndividuals() {
    return individuals;
}

const std::unordered_map<long, std::unique_ptr<Individual>>&
Population::getIndividuals() const {
    return individuals;
}
