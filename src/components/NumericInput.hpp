#pragma once
#include "Button.hpp"
#include "InteractiveElement.hpp"
#include <vector>
#include <string>

class NumericInput : public InteractiveElement {
protected:
    int minValue;
    int maxValue;
    int value;
    bool focused = false;
    std::string buffer;

    Button upButton;
    Button downButton;

    sf::Text valueText;
    sf::CircleShape triangleIconUp;
    sf::CircleShape triangleIconDown;

    sf::RectangleShape inputBody;
    sf::RectangleShape inputMarginDark1;
    sf::RectangleShape inputMarginDark2;
    sf::RectangleShape inputMarginLight1;
    sf::RectangleShape inputMarginLight2;
    sf::RectangleShape inputPaddingDark1;
    sf::RectangleShape inputPaddingDark2;
    sf::RectangleShape inputPaddingLight1;
    sf::RectangleShape inputPaddingLight2;

    void setupTrianglesIcons(std::pair<int,int> size, std::pair<int,int> pos);
    void setupInputBody();
    void setupInputPadding();
    void setupInputMargins();
    void setupValueText();

public:
    NumericInput(int minValue, int maxValue, Theme& theme, std::pair<int,int> size);
    NumericInput(int minValue, int maxValue, Theme& theme, std::pair<int,int> size, std::pair<int,int> pos);
    NumericInput(int minValue, int maxValue, int value, Theme& theme, std::pair<int,int> size, std::pair<int,int> pos);
    NumericInput(int minValue, int maxValue, int value, Theme &theme, std::pair<int, int> size);

    void setPosition(std::pair<int, int> pos) override;
    void setSize(std::pair<int,int> size) override;

    int getValue();

    void draw(sf::RenderWindow& window) override;

    bool handleFocus(sf::Event& event, sf::RenderWindow& window);
    void handleKeyboardInput(sf::Event& event);

    bool isUpButtonClicked(sf::Event& event, sf::RenderWindow& window);
    bool isDownButtonClicked(sf::Event& event, sf::RenderWindow& window);
};
