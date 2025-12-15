#include "Theme.hpp"
#include "../utils/ColorUtils.hpp"

// Constructores

Theme::Theme()
    : componentColor(sf::Color(60, 60, 60)),
      mainColor(sf::Color(50, 120, 200)),
      secondaryColor(sf::Color(100, 180, 140)),
      inputColor(sf::Color(45, 45, 45)),
      fontColor(sf::Color::White) {

    generateComponentVariations();
    generateMainVariations();
    generateSecondaryVariations();
    generateInputVariations();
}

Theme::Theme(const sf::Color& component,
             const sf::Color& main,
             const sf::Color& secondary,
             const sf::Color& input,
             const sf::Color& fontC,
             const sf::Font& f)
    : componentColor(component),
      mainColor(main),
      secondaryColor(secondary),
      inputColor(input),
      fontColor(fontC),
      font(f) {

    generateComponentVariations();
    generateMainVariations();
    generateSecondaryVariations();
    generateInputVariations();
}

// Generadores internos

void Theme::generateComponentVariations() {
    darkColor    = ColorUtils::darker(componentColor, 0.85f);
    darkerColor  = ColorUtils::darker(componentColor, 0.6f);
    lightColor   = ColorUtils::lighter(componentColor, 1.15f);
    lighterColor = ColorUtils::lighter(componentColor, 1.35f);
}

void Theme::generateMainVariations() {
    mainDarkColor    = ColorUtils::darker(mainColor, 0.85f);
    mainDarkerColor  = ColorUtils::darker(mainColor, 0.6f);
    mainLightColor   = ColorUtils::lighter(mainColor, 1.15f);
    mainLighterColor = ColorUtils::lighter(mainColor, 1.35f);
}

void Theme::generateSecondaryVariations() {
    secondaryDarkColor    = ColorUtils::darker(secondaryColor, 0.85f);
    secondaryDarkerColor  = ColorUtils::darker(secondaryColor, 0.6f);
    secondaryLightColor   = ColorUtils::lighter(secondaryColor, 1.15f);
    secondaryLighterColor = ColorUtils::lighter(secondaryColor, 1.35f);
}

void Theme::generateInputVariations() {
    inputDarkColor    = ColorUtils::darker(inputColor, 0.85f);
    inputDarkerColor  = ColorUtils::darker(inputColor, 0.6f);
    inputLightColor   = ColorUtils::lighter(inputColor, 1.15f);
    inputLighterColor = ColorUtils::lighter(inputColor, 1.35f);
}

// Getters

const sf::Color& Theme::getComponentColor() const { return componentColor; }
const sf::Color& Theme::getComponentDarkColor() const { return darkColor; }
const sf::Color& Theme::getComponentLightColor() const { return lightColor; }
const sf::Color& Theme::getComponentDarkerColor() const { return darkerColor; }
const sf::Color& Theme::getComponentLighterColor() const { return lighterColor; }

const sf::Color& Theme::getMainColor() const { return mainColor; }
const sf::Color& Theme::getMainDarkColor() const { return mainDarkColor; }
const sf::Color& Theme::getMainLightColor() const { return mainLightColor; }
const sf::Color& Theme::getMainDarkerColor() const { return mainDarkerColor; }
const sf::Color& Theme::getMainLighterColor() const { return mainLighterColor; }

const sf::Color& Theme::getSecondaryColor() const { return secondaryColor; }
const sf::Color& Theme::getSecondaryDarkColor() const { return secondaryDarkColor; }
const sf::Color& Theme::getSecondaryLightColor() const { return secondaryLightColor; }
const sf::Color& Theme::getSecondaryDarkerColor() const { return secondaryDarkerColor; }
const sf::Color& Theme::getSecondaryLighterColor() const { return secondaryLighterColor; }

const sf::Color& Theme::getInputColor() const { return inputColor; }
const sf::Color& Theme::getInputDarkColor() const { return inputDarkColor; }
const sf::Color& Theme::getInputLightColor() const { return inputLightColor; }
const sf::Color& Theme::getInputDarkerColor() const { return inputDarkerColor; }
const sf::Color& Theme::getInputLighterColor() const { return inputLighterColor; }

const sf::Color& Theme::getFontColor() const { return fontColor; }
const sf::Font& Theme::getFont() const { return font; }

// 🔹 Setters

void Theme::setComponentColor(const sf::Color& color) {
    componentColor = color;
    generateComponentVariations();
}

void Theme::setMainColor(const sf::Color& color) {
    mainColor = color;
    generateMainVariations();
}

void Theme::setSecondaryColor(const sf::Color& color) {
    secondaryColor = color;
    generateSecondaryVariations();
}

void Theme::setInputColor(const sf::Color& color) {
    inputColor = color;
    generateInputVariations();
}

void Theme::setFontColor(const sf::Color& color) {
    fontColor = color;
}

void Theme::setFont(const sf::Font& f) {
    font = f;
}
