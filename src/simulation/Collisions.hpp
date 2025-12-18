#pragma once

#include <unordered_set>
#include <cstdint>
#include <SFML/Graphics.hpp>

class Collisions {
private:
    std::unordered_set<uint64_t> cells;

    // Codifica (x,y) en uint64
    static uint64_t encode(int x, int y);

public:
    // --- Básico ---
    void clear();
    bool contains(int x, int y) const;

    std::unordered_set<uint64_t>& data();
    const std::unordered_set<uint64_t>& data() const;

    // --- Queries ---
    bool checkVertical(
        int left, int right,
        int y
    ) const;

    bool checkHorizontal(
        int top, int bottom,
        int x
    ) const;

    // --- Agregar colisiones ---
    void addCell(int x, int y);
    void addBox(const sf::FloatRect& box);
    void addShapePixels(const sf::Shape &shape);
    void addShape(const sf::Shape &shape, bool pixelPerfect);

    // --- Eliminar colisiones ---
    void removeShapePixels(const sf::Shape& shape);
    void removeBox(const sf::FloatRect& box);

    // --- Grupos ---
    template <typename Container>
    void addShapes(const Container& shapes);
};
