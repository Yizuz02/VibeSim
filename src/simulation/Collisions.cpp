#include "Collisions.hpp"
#include <cmath>

bool pointInShape(const sf::Shape& shape, float x, float y) {
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

// ---------------- ENCODE ----------------
uint64_t Collisions::encode(int x, int y) {
    return (uint64_t(x) << 32) | uint32_t(y);
}

// ---------------- BÁSICO ----------------
void Collisions::clear() {
    cells.clear();
}

bool Collisions::contains(int x, int y) const {
    return cells.count(encode(x, y)) > 0;
}

std::unordered_set<uint64_t>& Collisions::data() {
    return cells;
}

const std::unordered_set<uint64_t>& Collisions::data() const {
    return cells;
}

// ---------------- AGREGAR UNA CELDA ----------------
void Collisions::addCell(int x, int y) {
    cells.insert(encode(x, y));
}


// ---------------- CHECK VERTICAL ----------------
bool Collisions::checkVertical(int left, int right, int y) const {
    for (int x = left; x <= right; x++) {
        if (cells.count(encode(x, y)))
            return true;
    }
    return false;
}

// ---------------- CHECK HORIZONTAL ----------------
bool Collisions::checkHorizontal(int top, int bottom, int x) const {
    for (int y = top; y <= bottom; y++) {
        if (cells.count(encode(x, y)))
            return true;
    }
    return false;
}

// ---------------- REMOVE BOX ----------------
void Collisions::removeBox(const sf::FloatRect& box) {
    int left   = std::floor(box.left);
    int right  = std::floor(box.left + box.width  - 1);
    int top    = std::floor(box.top);
    int bottom = std::floor(box.top  + box.height - 1);

    for (int x = left; x <= right + 1; x++) {
        cells.erase(encode(x, top));
        cells.erase(encode(x, bottom));
        cells.erase(encode(x, top - 1));
        cells.erase(encode(x, bottom + 1));
    }

    for (int y = top; y <= bottom; y++) {
        cells.erase(encode(left, y));
        cells.erase(encode(right, y));
        cells.erase(encode(left - 1, y));
        cells.erase(encode(right + 1, y));
    }
}

void Collisions::removeShapePixels(const sf::Shape& shape) {
    sf::FloatRect bounds = shape.getGlobalBounds();

    int left   = std::floor(bounds.left);
    int right  = std::ceil (bounds.left + bounds.width);
    int top    = std::floor(bounds.top);
    int bottom = std::ceil (bounds.top  + bounds.height);

    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            if (pointInShape(shape, x + 0.5f, y + 0.5f)) {
                cells.erase(encode(x, y));
            }
        }
    }
}

// ---------------- ADD BOX ----------------
void Collisions::addBox(const sf::FloatRect& box) {
    int left   = std::floor(box.left);
    int right  = std::floor(box.left + box.width  - 1);
    int top    = std::floor(box.top);
    int bottom = std::floor(box.top  + box.height - 1);

    for (int x = left; x <= right; x++) {
        cells.insert(encode(x, top));
        cells.insert(encode(x, bottom));
    }

    for (int y = top; y <= bottom; y++) {
        cells.insert(encode(left, y));
        cells.insert(encode(right, y));
    }
}

// ---------------- ADD SHAPE ----------------
void Collisions::addShapePixels(const sf::Shape& shape) {
    sf::FloatRect bounds = shape.getGlobalBounds();

    int left   = std::floor(bounds.left);
    int right  = std::ceil (bounds.left + bounds.width);
    int top    = std::floor(bounds.top);
    int bottom = std::ceil (bounds.top  + bounds.height);

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            if (pointInShape(shape, x + 0.5f, y + 0.5f)) {
                cells.insert(encode(x, y));
            }
        }
    }
}


// ---------------- AGREGAR SHAPE ----------------
void Collisions::addShape(const sf::Shape& shape, bool pixelPerfect) {
    if (pixelPerfect) {
        addShapePixels(shape);
    } else {
        addBox(shape.getGlobalBounds());
    }
}
