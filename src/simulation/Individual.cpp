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

    // Intenta un desplazamiento (cx, cy): valida limites y colisiones y,
    // si es posible, mueve y actualiza la rejilla.
    auto tryMove = [&](float cx, float cy) -> bool {
        sf::FloatRect current = individualBody.getGlobalBounds();
        sf::FloatRect next = current;
        next.left += cx;
        next.top  += cy;

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
        if (cy > 0 && collisions.checkVertical(left, right, bottom + 1))
            return false;

        if (cy < 0 && collisions.checkVertical(left, right, top - 1))
            return false;

        // --- COLISIÓN HORIZONTAL ---
        if (cx > 0 && collisions.checkHorizontal(top, bottom, right + 1))
            return false;

        if (cx < 0 && collisions.checkHorizontal(top, bottom, left - 1))
            return false;

        // --- ACTUALIZAR COLISIONES ---
        if (oldLeft != left || oldTop != top ||
            oldRight != right || oldBottom != bottom) {

            collisions.removeBox(current);
            collisions.addBox(next);
        }

        individualBody.move(cx, cy);
        return true;
    };

    // 1) Desplazamiento completo.
    if (tryMove(dx, dy)) return true;

    // 2) Deslizamiento por ejes: si el movimiento combinado esta bloqueado,
    //    se intenta mover solo en X y, si no, solo en Y. Esto permite
    //    deslizarse a lo largo de obstaculos y de otros individuos en lugar
    //    de quedar atascado.
    if (dx != 0 && tryMove(dx, 0)) return true;
    if (dy != 0 && tryMove(0, dy)) return true;

    // 3) Evasión lateral: si el vector combinado y los dos deslizamientos
    //    axiales fallan (p. ej. un bloqueo diagonal con otro individuo
    //    delante), se intenta desplazarse a lo largo de la normal del vector
    //    de movimiento (dx,dy), en ambos sentidos y con magnitudes
    //    decrecientes, para apartarse lateralmente en lugar de quedar atascado.
    float len = std::hypot(dx, dy);
    if (len > 0.f) {
        float nx = -dy / len;
        float ny =  dx / len;

        const float magnitudes[3] = { len * 1.0f, len * 0.6f, len * 0.3f };
        for (float mag : magnitudes) {
            if (tryMove(nx * mag, ny * mag)) return true;
            if (tryMove(-nx * mag, -ny * mag)) return true;
        }

        // Combos: avanzar la mitad del vector original a la vez que se
        // desplaza la mitad de la magnitud en la normal, en ambos sentidos.
        float half = len * 0.5f;
        if (tryMove(dx * 0.5f + nx * half, dy * 0.5f + ny * half)) return true;
        if (tryMove(dx * 0.5f - nx * half, dy * 0.5f - ny * half)) return true;
    }

    return false;
}


long Individual::getId() const {
    return id;
}

sf::CircleShape& Individual::getShape() {
    return individualBody;
}

void Individual::setTheme(Theme *theme){
    this->theme = theme;
    individualBody.setFillColor(theme->getMainColor());
}
