#pragma once
#include <SFML/Graphics.hpp>

class Theme {
private:
    // 🔹 Colores base
    sf::Color componentColor;
    sf::Color mainColor;
    sf::Color secondaryColor;
    sf::Color fontColor;
    sf::Color inputColor;

    // 🔹 Variaciones component
    sf::Color darkColor;
    sf::Color lightColor;
    sf::Color darkerColor;
    sf::Color lighterColor;

    // 🔹 Variaciones main
    sf::Color mainDarkColor;
    sf::Color mainLightColor;
    sf::Color mainDarkerColor;
    sf::Color mainLighterColor;

    // 🔹 Variaciones secondary
    sf::Color secondaryDarkColor;
    sf::Color secondaryLightColor;
    sf::Color secondaryDarkerColor;
    sf::Color secondaryLighterColor;

    // 🔹 Variaciones input
    sf::Color inputDarkColor;
    sf::Color inputLightColor;
    sf::Color inputDarkerColor;
    sf::Color inputLighterColor;

    // 🔹 Fuente
    sf::Font font;

    // Helpers internos
    void generateComponentVariations();
    void generateMainVariations();
    void generateSecondaryVariations();
    void generateInputVariations();

public:
    // 🔹 Constructores
    Theme();
    Theme(const sf::Color& componentColor,
          const sf::Color& mainColor,
          const sf::Color& secondaryColor,
          const sf::Color& inputColor,
          const sf::Color& fontColor,
          const sf::Font& font);

    // 🔹 Getters
    const sf::Color& getComponentColor() const;
    const sf::Color& getComponentDarkColor() const;
    const sf::Color& getComponentLightColor() const;
    const sf::Color& getComponentDarkerColor() const;
    const sf::Color& getComponentLighterColor() const;

    const sf::Color& getMainColor() const;
    const sf::Color& getMainDarkColor() const;
    const sf::Color& getMainLightColor() const;
    const sf::Color& getMainDarkerColor() const;
    const sf::Color& getMainLighterColor() const;

    const sf::Color& getSecondaryColor() const;
    const sf::Color& getSecondaryDarkColor() const;
    const sf::Color& getSecondaryLightColor() const;
    const sf::Color& getSecondaryDarkerColor() const;
    const sf::Color& getSecondaryLighterColor() const;

    const sf::Color& getInputColor() const;
    const sf::Color& getInputDarkColor() const;
    const sf::Color& getInputLightColor() const;
    const sf::Color& getInputDarkerColor() const;
    const sf::Color& getInputLighterColor() const;

    const sf::Color& getFontColor() const;
    const sf::Font& getFont() const;

    // 🔹 Setters
    void setComponentColor(const sf::Color& color);
    void setMainColor(const sf::Color& color);
    void setSecondaryColor(const sf::Color& color);
    void setInputColor(const sf::Color& color);
    void setFontColor(const sf::Color& color);
    void setFont(const sf::Font& font);
};
