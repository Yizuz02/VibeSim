#include "ColorUtils.hpp"
#include <algorithm>

namespace ColorUtils {

    sf::Color darker(const sf::Color& color, float factor) {
        // Asegura que sea < 1.0f
        if (factor >= 1.f)
            factor = 0.7f;

        return sf::Color(
            static_cast<sf::Uint8>(color.r * factor),
            static_cast<sf::Uint8>(color.g * factor),
            static_cast<sf::Uint8>(color.b * factor),
            color.a
        );
    }

    sf::Color lighter(const sf::Color& color, float factor) {
        // Asegura que sea > 1.0f
        if (factor <= 1.f)
            factor = 1.3f;

        if (color.r == 0 && color.g == 0 && color.b==0){
            return sf::Color(
            static_cast<sf::Uint8>(std::min(255, int(10 * factor))),
            static_cast<sf::Uint8>(std::min(255, int(10 * factor))),
            static_cast<sf::Uint8>(std::min(255, int(10 * factor))),
            color.a
            );
        } else {
            return sf::Color(
            static_cast<sf::Uint8>(std::min(255, int(color.r * factor))),
            static_cast<sf::Uint8>(std::min(255, int(color.g * factor))),
            static_cast<sf::Uint8>(std::min(255, int(color.b * factor))),
            color.a
            );
        }
         

        
    }

    sf::Color colorFromJson(const json& j)
    {
        return sf::Color(
            j.at(0).get<uint8_t>(),
            j.at(1).get<uint8_t>(),
            j.at(2).get<uint8_t>()
        );
    }
}