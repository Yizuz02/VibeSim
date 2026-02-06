#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Space.hpp"
#include "../styles/Theme.hpp"

class Targets {
public:
    struct Target {
        long id;
        std::unique_ptr<sf::CircleShape> shape;
    };

    Targets(Space& space, const sf::Color& color);

    long addTarget(std::pair<float,float> position, float radius);
    long addTarget(float radius);

    long getTargetAt(float x, float y) const;

    Target* getTarget(long id);
    const Target* getTarget(long id) const;
    const std::vector<Target>& getTargets() const;

    bool removeTarget(long id);
    void clear();

    void setColor(const sf::Color& color);
    void draw();

private:
    Space* space;
    sf::Color color;

    std::vector<Target> targets;
    long nextId = 0;
};
