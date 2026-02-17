#include "Targets.hpp"
#include <random>
#include <cmath>

// ---------- CONSTRUCTOR ----------
Targets::Targets(Space& space, const sf::Color& color)
    : space(&space), color(color) {}


// ---------- AGREGAR CON POSICIÓN ----------
long Targets::addTarget(std::pair<float,float> position, float radius) {

    auto circle = std::make_unique<sf::CircleShape>(radius);
    circle->setPosition(position.first, position.second);
    circle->setFillColor(color);

    sf::FloatRect bounds = circle->getGlobalBounds();

    if (bounds.left < space->minX() ||
        bounds.top < space->minY() ||
        bounds.left + bounds.width  > space->maxX() ||
        bounds.top  + bounds.height > space->maxY())
        return -1;

    Collisions& collisions = space->getCollisions();

    int left   = std::floor(bounds.left);
    int right  = std::ceil (bounds.left + bounds.width);
    int top    = std::floor(bounds.top);
    int bottom = std::ceil (bounds.top  + bounds.height);

    

    targets.push_back({ nextId, std::move(circle) });
    return nextId++;
}


// ---------- AGREGAR ALEATORIO ----------
long Targets::addTarget(float radius) {

    auto circle = std::make_unique<sf::CircleShape>(radius);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distx(
        space->minX() + radius,
        space->maxX() - radius * 2
    );
    std::uniform_int_distribution<int> disty(
        space->minY() + radius,
        space->maxY() - radius * 2
    );

    Collisions& collisions = space->getCollisions();
    constexpr int MAX_ATTEMPTS = 3000;

    for (int i = 0; i < MAX_ATTEMPTS; ++i) {

        circle->setPosition(distx(gen), disty(gen));
        sf::FloatRect bounds = circle->getGlobalBounds();

        int left   = std::floor(bounds.left);
        int right  = std::ceil (bounds.left + bounds.width);
        int top    = std::floor(bounds.top);
        int bottom = std::ceil (bounds.top  + bounds.height);

        bool collision = false;

        for (int y = top; y <= bottom && !collision; ++y)
            for (int x = left; x <= right; ++x)
                if (collisions.contains(x, y)) {
                    collision = true;
                    break;
                }

        if (!collision) {
            circle->setFillColor(color);
            targets.push_back({ nextId, std::move(circle) });
            return nextId++;
        }
    }

    return -1;
}


// ---------- SELECCIÓN ----------
long Targets::getTargetAt(float x, float y) const {
    for (const auto& t : targets) {

        const sf::CircleShape* circle =
            dynamic_cast<const sf::CircleShape*>(t.shape.get());

        if (!circle) continue;

        float radius = circle->getRadius();
        sf::Vector2f center = circle->getPosition();

        // Si el origen no está centrado:
        center += sf::Vector2f(radius, radius);

        float dx = x - center.x;
        float dy = y - center.y;

        if (dx * dx + dy * dy <= radius * radius)
            return t.id;
    }
    return -1;
}



// ---------- ACCESO ----------
Targets::Target* Targets::getTarget(long id) {
    for (auto& t : targets)
        if (t.id == id) return &t;
    return nullptr;
}

const Targets::Target* Targets::getTarget(long id) const {
    for (const auto& t : targets)
        if (t.id == id) return &t;
    return nullptr;
}

const std::vector<Targets::Target>& Targets::getTargets() const {
    return targets;
}

bool Targets::empty() const {
    return targets.empty();
}

// ---------- COLOR ----------
void Targets::setColor(const sf::Color& color) {
    this->color = color;
    for (auto& t : targets)
        t.shape->setFillColor(color);
}


// ---------- ELIMINAR ----------
bool Targets::removeTarget(long id) {
    for (auto it = targets.begin(); it != targets.end(); ++it)
        if (it->id == id) {
            targets.erase(it);
            return true;
        }
    return false;
}

void Targets::clear() {
    targets.clear();
}


// ---------- RENDER ----------
void Targets::draw() {
    for (auto& t : targets)
        space->getWindow().draw(*t.shape);
}

