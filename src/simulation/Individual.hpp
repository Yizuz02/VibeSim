#pragma once

#include <SFML/Graphics.hpp>
#include <utility>
#include <unordered_set>
#include <cstdint>
#include "Space.hpp"
#include "../styles/Theme.hpp"

class Individual {
private:
    long id;
    std::pair<int,int> pos;
    float radius;
    sf::CircleShape individualBody;
    Theme* theme;
    Space* space;

public:
    // Constructor
    Individual(long id,
               float radius,
               std::pair<int,int> pos,
               Space& space,
               Theme& theme);

    // Render
    void draw();

    // Movimiento con colisiones
    bool move(float dx, float dy);

    // Getters
    long getId() const;
    sf::CircleShape& getShape();
};
