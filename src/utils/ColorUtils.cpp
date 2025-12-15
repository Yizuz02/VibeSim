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

        return sf::Color(
            static_cast<sf::Uint8>(std::min(255, int(color.r * factor))),
            static_cast<sf::Uint8>(std::min(255, int(color.g * factor))),
            static_cast<sf::Uint8>(std::min(255, int(color.b * factor))),
            color.a
        );
    }

}