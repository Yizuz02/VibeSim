#include "Space.hpp"

Space::Space(std::pair<int,int> pos,
             std::pair<int,int> size,
             Theme& theme,
             Collisions& collisions,
             sf::RenderWindow& window)
    : pos(pos),
      size(size),
      topBoundary(pos.second),
      theme(&theme),
      window(window),
      collisions(&collisions)
{
    background.setSize(sf::Vector2f(size.first, size.second));
    background.setFillColor(theme.getBackgroundColor());
    background.setPosition(pos.first, pos.second);
}

Space::Space(int topBoundary,
             std::pair<int,int> size,
             Theme& theme,
             Collisions& collisions,
             sf::RenderWindow& window)
    : size(size),
      topBoundary(topBoundary),
      theme(&theme),
      window(window),
      collisions(&collisions)
{
    background.setFillColor(theme.getSpaceColor());
    setSize(size);
}


void Space::setPosition(std::pair<int,int> pos) {
    this->pos = pos;
    background.setPosition(pos.first, pos.second);
}

void Space::setSize(std::pair<int,int> size) {
    this->size = size;
    background.setSize(
        sf::Vector2f(static_cast<float>(size.first),
                     static_cast<float>(size.second))
    );
   
    sf::View view = window.getView();
    sf::Vector2f center = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    int x = static_cast<int>(center.x - size.first / 2.f);
    int y = static_cast<int>(center.y - (size.second + topBoundary) / 2.f + topBoundary);

    setPosition({x, y});
}

void Space::setTheme(Theme &theme){
    this->theme = &theme;
    background.setFillColor(theme.getSpaceColor());
}

void Space::setTopBoundary(int topBoundary) {
    this->topBoundary = topBoundary;
}

std::pair<int,int> Space::getPosition() const {
    return pos;
}

std::pair<int,int> Space::getSize() const {
    return size;
}

int Space::getTopBoundary() const {
    return topBoundary;
}

sf::RenderWindow& Space::getWindow() const{
    return window;
}

Collisions& Space::getCollisions() {
    return *collisions;
}

const Collisions& Space::getCollisions() const {
    return *collisions;
}


float Space::minX() const {
    return pos.first;
}

float Space::minY() const {
    return pos.second;
}

float Space::maxX() const {
    return pos.first + size.first;
}

float Space::maxY() const {
    return pos.second + size.second;
}

bool Space::contains(float x, float y, float radius) const {
    return
        x - radius >= minX() &&
        y - radius >= minY() &&
        x + radius <= maxX() &&
        y + radius <= maxY();
}

bool Space::contains(const sf::FloatRect& box) const {
    return
        box.left >= minX() &&
        box.top >= minY() &&
        box.left + box.width  <= maxX() &&
        box.top  + box.height <= maxY();
}

void Space::draw() {
    window.draw(background);
}





