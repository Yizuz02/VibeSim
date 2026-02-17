#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Collisions.hpp"
#include "Space.hpp"
#include "../styles/Theme.hpp"

class Obstacles {
private:
    struct Obstacle {
        long id;
        std::unique_ptr<sf::Shape> shape;
    };

    std::vector<Obstacle> obstacles;
    long nextId = 0;

    Space* space;
    Theme* theme;

public:
    Obstacles(Space& space, Theme& theme);

    // ---------------- CREAR ----------------
    long addRegularPolygon(
        std::pair<float, float> position,
        float radius,
        unsigned int sides
    );

    long addConvexObstacle(sf::ConvexShape shape);

    long addRegularPolygon(float radius, unsigned int sides);

    // ---------------- SELECCIÓN ----------------
    long getObstacleAt(float x, float y) const;

    Obstacle* getObstacle(long id);
    const Obstacle* getObstacle(long id) const;
    sf::Shape& getShape();

    void setTheme(Theme& theme);

    // ---------------- ELIMINAR ----------------
    bool removeObstacle(long id);
    void clear();

    // ---------------- RENDER ----------------
    void draw();
};
