#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <cstdint>
#include <utility>
#include "Collisions.hpp"

class Space {
private:
    std::pair<int,int> pos;
    std::pair<int,int> size;
    int topBoundary;
    sf::Color backgroundColor;
    sf::RenderWindow& window;

    // Referencia a los obstáculos del mundo
    Collisions* collisions;

    // Fondo visual
    sf::RectangleShape background;
    

public:
    Space(std::pair<int,int> pos,
          std::pair<int,int> size,
          sf::Color backgroundColor,
          Collisions& collisions,
          sf::RenderWindow& window);

    Space(int minY,
          std::pair<int,int> size,
          sf::Color backgroundColor,
          Collisions& collisions,
          sf::RenderWindow& window);

    void setPosition(std::pair<int,int> pos);
    void setSize(std::pair<int,int> size);
    void setBackgroundColor(const sf::Color& color);
    void setTopBoundary(int minY);

    std::pair<int,int> getPosition() const;
    std::pair<int,int> getSize() const;
    int getTopBoundary() const;
    sf::Color getBackgroundColor() const;
    sf::RenderWindow& getWindow() const;
    Collisions& getCollisions();
    const Collisions& getCollisions() const;

    float minX() const;
    float minY() const;
    float maxX() const;
    float maxY() const;

    // Render
    void draw();
};
