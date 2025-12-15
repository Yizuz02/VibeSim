#pragma once
#include <SFML/Graphics.hpp>

namespace ColorUtils {

    sf::Color darker(const sf::Color& color, float factor = 0.7f);
    sf::Color lighter(const sf::Color& color, float factor = 1.3f);

}
