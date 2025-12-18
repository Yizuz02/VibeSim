#include "Obstacles.hpp"
#include <cmath>
#include <random>

bool pointInShape2(const sf::Shape& shape, float x, float y) {
    // Transformar a espacio local del shape
    sf::Vector2f local =
        shape.getInverseTransform().transformPoint(x, y);

    std::size_t count = shape.getPointCount();
    bool inside = false;

    for (std::size_t i = 0, j = count - 1; i < count; j = i++) {
        sf::Vector2f pi = shape.getPoint(i);
        sf::Vector2f pj = shape.getPoint(j);

        bool intersect =
            ((pi.y > local.y) != (pj.y > local.y)) &&
            (local.x < (pj.x - pi.x) * (local.y - pi.y) / (pj.y - pi.y) + pi.x);

        if (intersect)
            inside = !inside;
    }
    return inside;
}


// ---------------- CONSTRUCTOR ----------------
Obstacles::Obstacles(Space& space, Theme& theme)
    : space(&space), theme(&theme) {}


// ---------------- AGREGAR ----------------
long Obstacles::addRegularPolygon(
    std::pair<float, float> position,
    float radius,
    unsigned int sides
) {
    auto poly = std::make_unique<sf::CircleShape>(radius, sides);

    poly->setPosition(position.first, position.second);
    poly->setFillColor(theme->getSecondaryColor());

    // Registrar colisiones
    space->getCollisions().addShape(*poly, true);

    obstacles.push_back({ nextId, std::move(poly) });
    return nextId++;
}


long Obstacles::addRegularPolygon(
    float radius,
    unsigned int sides
) {
    auto poly = std::make_unique<sf::CircleShape>(radius, sides);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disty(
        space->minY() + radius,
        space->maxY() - radius * 2
    );
    std::uniform_int_distribution<int> distx(
        space->minX() + radius,
        space->maxX() - radius * 2
    );

    Collisions& collisions = space->getCollisions();

    constexpr int MAX_ATTEMPTS = 5000;
    std::pair<int,int> pos;
    sf::FloatRect box;
    bool valid = false;

    for (int i = 0; i < MAX_ATTEMPTS; ++i) {
        int px = distx(gen);
        int py = disty(gen);

        poly->setPosition(static_cast<float>(px),
                          static_cast<float>(py));

        sf::FloatRect bounds = poly->getGlobalBounds();

        int left   = std::floor(bounds.left);
        int right  = std::ceil (bounds.left + bounds.width);
        int top    = std::floor(bounds.top);
        int bottom = std::ceil (bounds.top  + bounds.height);

        bool collision = false;

        for (int y = top; y <= bottom && !collision; ++y) {
            for (int x = left; x <= right; ++x) {

                if (pointInShape2(*poly, x + 0.5f, y + 0.5f) &&
                    collisions.contains(x, y)) {
                    collision = true;
                    break;
                }
            }
        }

        if (!collision) {
            valid = true;
            break;
        }
        break;
    }

    if (!valid) {
        return -1;
    }

    poly->setFillColor(theme->getSecondaryColor());

    // 🔥 ahora sí, registrar colisiones
    collisions.addShape(*poly, true);

    obstacles.push_back({ nextId, std::move(poly) });
    return nextId++;
}


// ---------------- SELECCIONAR POR POSICIÓN ----------------
long Obstacles::getObstacleAt(float x, float y) const {
    for (const auto& obs : obstacles) {
        if (obs.shape->getGlobalBounds().contains(x, y)) {
            return obs.id;
        }
    }
    return -1;
}


// ---------------- ACCESO POR ID ----------------
Obstacles::Obstacle* Obstacles::getObstacle(long id) {
    for (auto& obs : obstacles)
        if (obs.id == id)
            return &obs;
    return nullptr;
}

const Obstacles::Obstacle* Obstacles::getObstacle(long id) const {
    for (const auto& obs : obstacles)
        if (obs.id == id)
            return &obs;
    return nullptr;
}

// ---------------- ELIMINAR UNO ----------------
bool Obstacles::removeObstacle(long id) {
    for (auto it = obstacles.begin(); it != obstacles.end(); ++it) {
        if (it->id == id) {
            space->getCollisions().removeShapePixels(*it->shape);
            obstacles.erase(it);
            return true;
        }
    }
    return false;
}


// ---------------- ELIMINAR TODOS ----------------
void Obstacles::clear() {
    Collisions& collisions = space->getCollisions();

    for (auto& obs : obstacles) {
        collisions.removeShapePixels(*obs.shape);
    }

    obstacles.clear();
}

// ---------------- RENDER ----------------
void Obstacles::draw() {
    for (auto& obs : obstacles) {
        space->getWindow().draw(*obs.shape);
    }
}
