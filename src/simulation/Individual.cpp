#include "Individual.hpp"
#include <cmath>

Individual::Individual(long id,
                       float radius,
                       std::pair<int,int> pos,
                       Space& space,
                       Theme& theme)
    : id(id),
      pos(pos),
      radius(radius),
      space(&space),
      theme(&theme)
{
    individualBody.setRadius(radius);
    individualBody.setPosition(
        sf::Vector2f(static_cast<float>(pos.first),
                     static_cast<float>(pos.second))
    );
    individualBody.setFillColor(theme.getMainColor());
}

void Individual::draw() {
    space->getWindow().draw(individualBody);
}

bool Individual::move(float dx, float dy)
{
    if (dx == 0 && dy == 0) return true;

    Collisions& collisions = space->getCollisions();

    sf::FloatRect current = individualBody.getGlobalBounds();
    sf::FloatRect next = current;
    next.left += dx;
    next.top  += dy;

    int oldLeft   = std::floor(current.left);
    int oldRight  = std::floor(current.left + current.width  - 1);
    int oldTop    = std::floor(current.top);
    int oldBottom = std::floor(current.top  + current.height - 1);

    int left   = std::floor(next.left);
    int right  = std::floor(next.left + next.width  - 1);
    int top    = std::floor(next.top);
    int bottom = std::floor(next.top  + next.height - 1);

    // --- LÍMITES ---
    if (left < space->minX() || top < space->minY()) return false;
    if (right > space->maxX() || bottom > space->maxY()) return false;

    // --- COLISIÓN VERTICAL ---
    if (dy > 0 && collisions.checkVertical(left, right, bottom + 1))
        return false;

    if (dy < 0 && collisions.checkVertical(left, right, top - 1))
        return false;

    // --- COLISIÓN HORIZONTAL ---
    if (dx > 0 && collisions.checkHorizontal(top, bottom, right + 1))
        return false;

    if (dx < 0 && collisions.checkHorizontal(top, bottom, left - 1))
        return false;

    // --- ACTUALIZAR COLISIONES ---
    if (oldLeft != left || oldTop != top ||
        oldRight != right || oldBottom != bottom) {

        collisions.removeBox(current);
        collisions.addBox(next);
    }

    individualBody.move(dx, dy);
    return true;
}


long Individual::getId() const {
    return id;
}

sf::CircleShape& Individual::getShape() {
    return individualBody;
}
