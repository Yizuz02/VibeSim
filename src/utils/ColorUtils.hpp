#pragma once
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ColorUtils {

    sf::Color darker(const sf::Color& color, float factor = 0.7f);
    sf::Color lighter(const sf::Color& color, float factor = 1.3f);
    sf::Color colorFromJson(const json& j);
}
