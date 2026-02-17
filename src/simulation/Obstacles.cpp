#include "Obstacles.hpp"
#include <cmath>
#include <random>
#include<iostream>

bool pointInShape2(const sf::Shape& shape, float x, float y) {

    sf::Vector2f local =
        shape.getInverseTransform().transformPoint(x, y);

    std::size_t count = shape.getPointCount();
    int windingNumber = 0;

    for (std::size_t i = 0; i < count; ++i) {

        sf::Vector2f p1 = shape.getPoint(i);
        sf::Vector2f p2 = shape.getPoint((i + 1) % count);

        if (p1.y <= local.y) {

            if (p2.y > local.y) {

                double cross =
                    (double)(p2.x - p1.x) * (local.y - p1.y) -
                    (double)(local.x - p1.x) * (p2.y - p1.y);

                if (cross > 0)
                    ++windingNumber;
            }

        } else {

            if (p2.y <= local.y) {

                double cross =
                    (double)(p2.x - p1.x) * (local.y - p1.y) -
                    (double)(local.x - p1.x) * (p2.y - p1.y);

                if (cross < 0)
                    --windingNumber;
            }
        }
    }

    return windingNumber != 0;
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
    poly->setFillColor(theme->getSecondaryDarkColor());

    // Registrar colisiones
    space->getCollisions().addShape(*poly, true);

    obstacles.push_back({ nextId, std::move(poly) });
    return nextId++;
}

long Obstacles::addConvexObstacle(sf::ConvexShape shape)
{
    auto poly = std::make_unique<sf::ConvexShape>(shape);

    poly->setFillColor(theme->getSecondaryDarkColor());

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

    constexpr int MAX_ATTEMPTS = 10000;
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

    poly->setFillColor(theme->getSecondaryDarkColor());
    collisions.addShape(*poly, true);

    obstacles.push_back({ nextId, std::move(poly) });
    return nextId++;
}


// ---------------- SELECCIONAR POR POSICIÓN ----------------
long Obstacles::getObstacleAt(float x, float y) const {
    for (const auto& obs : obstacles) {
        if (obs.shape->getGlobalBounds().contains(x, y)) {
            if(pointInShape2(*obs.shape, x, y))
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

void Obstacles::setTheme(Theme &theme){
    this->theme = &theme;
    for (const auto& obs : obstacles){
        obs.shape->setFillColor(theme.getSecondaryDarkColor());
    }
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