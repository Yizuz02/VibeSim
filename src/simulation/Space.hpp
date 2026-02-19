#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <cstdint>
#include <utility>
#include "Collisions.hpp"
#include "../styles/Theme.hpp"


class Space {
private:
    std::pair<int,int> pos;
    std::pair<int,int> size;
    int topBoundary;
    sf::RenderWindow& window;

    // Referencia a los obstáculos del mundo
    Collisions* collisions;

    // Fondo visual
    sf::RectangleShape background;
    Theme* theme;
    
public:
    Space(std::pair<int,int> pos,
          std::pair<int,int> size,
          Theme& theme,
          Collisions& collisions,
          sf::RenderWindow& window);

    Space(int minY,
          std::pair<int,int> size,
          Theme& theme,
          Collisions& collisions,
          sf::RenderWindow& window);

    void setPosition(std::pair<int,int> pos);
    void setSize(std::pair<int,int> size);
    void setTheme(Theme& theme);
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

    bool contains(float x, float y, float radius = 0.f) const;
    bool contains(const sf::FloatRect& box) const;

    // Render
    void draw();
};
