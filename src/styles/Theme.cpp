#include "Theme.hpp"
#include "../utils/ColorUtils.hpp"
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

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
             const sf::Color& spaceColor,
             const sf::Color& backgroundColor,
             const sf::Color& goalColor,
             const sf::Color& startColor,
             const sf::Color& input,
             const sf::Color& fontC,
             const sf::Font& f)
    : componentColor(component),
      mainColor(main),
      secondaryColor(secondary),
      spaceColor(spaceColor),
      backgroundColor(backgroundColor),
      goalColor(goalColor),
      inputColor(input),
      fontColor(fontC),
      font(f) {

    generateComponentVariations();
    generateMainVariations();
    generateSecondaryVariations();
    generateInputVariations();
    disabledFontColor = ColorUtils::lighter(fontColor, 10.f);
}

Theme Theme::loadFromJson(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo de tema: " + path);
    }

    json j;
    file >> j;

    // Colores
    sf::Color componentColor  = ColorUtils::colorFromJson(j.at("componentColor"));
    sf::Color mainColor       = ColorUtils::colorFromJson(j.at("mainColor"));
    sf::Color secondaryColor  = ColorUtils::colorFromJson(j.at("secondaryColor"));
    sf::Color backgroundColor = ColorUtils::colorFromJson(j.at("backgroundColor"));
    sf::Color spaceColor      = ColorUtils::colorFromJson(j.at("spaceColor"));
    sf::Color goalColor       = ColorUtils::colorFromJson(j.at("goalColor"));
    sf::Color startColor       = ColorUtils::colorFromJson(j.at("startColor"));
    sf::Color inputColor      = ColorUtils::colorFromJson(j.at("inputColor"));
    sf::Color fontColor       = ColorUtils::colorFromJson(j.at("fontColor"));

    // Fuente
    sf::Font font;
    std::string fontPath = j.at("fontPath").get<std::string>();

    if (!font.loadFromFile(fontPath)) {
        throw std::runtime_error("No se pudo cargar la fuente: " + fontPath);
    }

    // Construcción final
    return Theme(
        componentColor,
        mainColor,
        secondaryColor,
        spaceColor,
        backgroundColor,
        goalColor,
        startColor,
        inputColor,
        fontColor,
        font
    );
}

std::unordered_map<std::string, Theme>
Theme::loadAllFromFolder(const std::string& folderPath) {

    std::unordered_map<std::string, Theme> themes;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".json") {

            std::ifstream file(entry.path());
            json j;
            file >> j;

            std::string name = j.value("name", entry.path().stem().string());

            Theme theme = Theme::loadFromJson(entry.path().string());

            themes.emplace(name, std::move(theme));
        }
    }

    return themes;
}


// Generadores internos

void Theme::generateComponentVariations() {
    darkColor    = ColorUtils::darker(componentColor, 0.85f);
    darkerColor  = ColorUtils::darker(componentColor, 0.3f);
    lightColor   = ColorUtils::lighter(componentColor, 1.15f);
    lighterColor = ColorUtils::lighter(componentColor, 2.2f);
}

void Theme::generateMainVariations() {
    mainDarkColor    = ColorUtils::darker(mainColor, 0.85f);
    mainDarkerColor  = ColorUtils::darker(mainColor, 0.3f);
    mainLightColor   = ColorUtils::lighter(mainColor, 1.15f);
    mainLighterColor = ColorUtils::lighter(mainColor, 2.2f);
}

void Theme::generateSecondaryVariations() {
    secondaryDarkColor    = ColorUtils::darker(secondaryColor, 0.85f);
    secondaryDarkerColor  = ColorUtils::darker(secondaryColor, 0.3f);
    secondaryLightColor   = ColorUtils::lighter(secondaryColor, 1.15f);
    secondaryLighterColor = ColorUtils::lighter(secondaryColor, 2.2f);
}

void Theme::generateInputVariations() {
    inputDarkColor    = ColorUtils::darker(inputColor, 0.85f);
    inputDarkerColor  = ColorUtils::darker(inputColor, 0.3f);
    inputLightColor   = ColorUtils::lighter(inputColor, 1.15f);
    inputLighterColor = ColorUtils::lighter(inputColor, 2.2f);
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

const sf::Color &Theme::getBackgroundColor() const { return backgroundColor;}

const sf::Color &Theme::getSpaceColor() const { return spaceColor;}

const sf::Color &Theme::getGoalColor() const { return goalColor; }
const sf::Color &Theme::getStartColor() const { return startColor; }

const sf::Color& Theme::getFontColor() const { return fontColor; }
const sf::Color &Theme::getDisabledFontColor() const{ return disabledFontColor;}
const sf::Font &Theme::getFont() const { return font; }

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

void Theme::setBackgroundColor(const sf::Color &color){
    backgroundColor = color;
}

void Theme::setSpaceColor(const sf::Color &color){
    spaceColor = color;
}

void Theme::setFontColor(const sf::Color& color) {
    fontColor = color;
}

void Theme::setFont(const sf::Font& f) {
    font = f;
}
