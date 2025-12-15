#pragma once
#include <SFML/Graphics.hpp>
#include "InteractiveElement.hpp"
#include <string>
#include <utility>

class Button : public InteractiveElement{
protected:
    std::pair<int,int> pos;
    std::pair<int,int> size;

    bool isPressed = false;

    sf::RectangleShape buttonBody;
    sf::RectangleShape buttonMarginDark1;
    sf::RectangleShape buttonMarginDark2;
    sf::RectangleShape buttonMarginLight1;
    sf::RectangleShape buttonMarginLight2;
    sf::RectangleShape buttonPaddingDark1;
    sf::RectangleShape buttonPaddingDark2;
    sf::RectangleShape buttonPaddingLight1;
    sf::RectangleShape buttonPaddingLight2;

    sf::Text buttonText;

    void setupButtonBody(std::pair<int,int> size, std::pair<int,int> pos);
    void setupButtonPadding(std::pair<int,int> size, std::pair<int,int> pos);
    void setupButtonMargins(std::pair<int,int> size, std::pair<int,int> pos);
    void setupButtonText(std::string text, std::pair<int,int> pos);

public:
    Button(std::string buttonText, Theme& theme, std::pair<int,int> size, std::pair<int,int> pos);
    Button(std::string buttonText, Theme& theme, std::pair<int,int> size);
    Button();

    void setSize(std::pair<int,int> pos) override;
    void setPosition(std::pair<int,int> pos) override;
    void setButtonText(std::string text);

    std::string getButtonText();

    void draw(sf::RenderWindow& window) override;

    bool isClicked(sf::Event& event, sf::RenderWindow& window);
};
