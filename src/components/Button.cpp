#include "Button.hpp"

void Button::setupButtonBody(std::pair<int,int> size, std::pair<int,int> pos) {
    buttonBody.setSize(sf::Vector2f(size.first, size.second));
    buttonBody.setPosition(sf::Vector2f(pos.first, pos.second));
    buttonBody.setFillColor(theme.getComponentColor());
}

void Button::setupButtonPadding(std::pair<int,int> size, std::pair<int,int> pos) {
    buttonPaddingDark1.setSize(sf::Vector2f(2, size.second-2));
    buttonPaddingDark1.setPosition(sf::Vector2f(pos.first+size.first-4, pos.second));
    buttonPaddingDark1.setFillColor(theme.getComponentDarkColor());

    buttonPaddingDark2.setSize(sf::Vector2f(size.first-2, 2));
    buttonPaddingDark2.setPosition(sf::Vector2f(pos.first, pos.second+size.second-4));
    buttonPaddingDark2.setFillColor(theme.getComponentDarkColor());

    buttonPaddingLight1.setSize(sf::Vector2f(2, size.second-6));
    buttonPaddingLight1.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    buttonPaddingLight1.setFillColor(theme.getComponentLightColor());

    buttonPaddingLight2.setSize(sf::Vector2f(size.first-6, 2));
    buttonPaddingLight2.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    buttonPaddingLight2.setFillColor(theme.getComponentLightColor());
}

void Button::setupButtonMargins(std::pair<int,int> size, std::pair<int,int> pos) {
    buttonMarginDark1.setSize(sf::Vector2f(2, size.second));
    buttonMarginDark1.setPosition(sf::Vector2f(pos.first+size.first-2, pos.second));
    buttonMarginDark1.setFillColor(theme.getComponentDarkerColor());

    buttonMarginDark2.setSize(sf::Vector2f(size.first, 2));
    buttonMarginDark2.setPosition(sf::Vector2f(pos.first, pos.second+size.second-2));
    buttonMarginDark2.setFillColor(theme.getComponentDarkerColor());

    buttonMarginLight1.setSize(sf::Vector2f(2, size.second-2));
    buttonMarginLight1.setPosition(sf::Vector2f(pos.first, pos.second));
    buttonMarginLight1.setFillColor(theme.getComponentLighterColor());

    buttonMarginLight2.setSize(sf::Vector2f(size.first-2, 2));
    buttonMarginLight2.setPosition(sf::Vector2f(pos.first, pos.second));
    buttonMarginLight2.setFillColor(theme.getComponentLighterColor());
}

void Button::setupButtonText(std::string text, std::pair<int,int> pos) {
    int fontSize = size.second/2.5;
    buttonText.setFont(theme.getFont());
    buttonText.setString(text);
    buttonText.setCharacterSize(fontSize);
    buttonText.setPosition(sf::Vector2f(pos.first+8, pos.second+((size.second-buttonText.getGlobalBounds().height)/3)));
    buttonText.setFillColor(theme.getFontColor());
}

Button::Button(std::string buttonText,
               Theme& theme,
               std::pair<int,int> size,
               std::pair<int,int> pos)
    : InteractiveElement(theme, size, pos, "") {
        setupButtonBody(size, pos);
        setupButtonPadding(size, pos);
        setupButtonMargins(size, pos);
        setupButtonText(buttonText, pos);
}

Button::Button(std::string buttonText,
               Theme& theme,
               std::pair<int,int> size)
    : InteractiveElement(theme, size, "") {
        setupButtonBody(size, pos);
        setupButtonPadding(size, pos);
        setupButtonMargins(size, pos);
        setupButtonText(buttonText, pos);
}

void Button::setPosition(std::pair<int,int> pos) {
    this->pos = pos;
    setupButtonBody(size, pos);
    setupButtonPadding(size, pos);
    setupButtonMargins(size, pos);
    int fontSize = size.second/2.5;
    buttonText.setCharacterSize(fontSize);
    buttonText.setPosition(sf::Vector2f(pos.first+8, pos.second+((size.second-buttonText.getGlobalBounds().height)/3)));
}

void Button::setSize(std::pair<int,int> size) {
    this->size = size;
    setupButtonBody(size, pos);
    setupButtonPadding(size, pos);
    setupButtonMargins(size, pos);
    int fontSize = size.second/2.5;
    buttonText.setCharacterSize(fontSize);
    buttonText.setPosition(sf::Vector2f(pos.first+8, pos.second+((size.second-buttonText.getGlobalBounds().height)/3)));
}

void Button::setButtonText(std::string text){
    buttonText.setString(text);;
}


std::string Button::getButtonText(){
    return buttonText.getString();
}

void Button::draw(sf::RenderWindow& window) {
    if (visible){
        window.draw(buttonBody);
        window.draw(buttonText);
        window.draw(buttonMarginDark1);
        window.draw(buttonMarginDark2);
        window.draw(buttonPaddingDark1);
        window.draw(buttonPaddingDark2);
        window.draw(buttonMarginLight1);
        window.draw(buttonMarginLight2);
        window.draw(buttonPaddingLight1);
        window.draw(buttonPaddingLight2);
    }
}

bool Button::isClicked(sf::Event& event, sf::RenderWindow& window) {
    if(enabled){
        sf::Vector2f mouse = window.mapPixelToCoords(
            sf::Mouse::getPosition(window)
        );

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

            if (buttonBody.getGlobalBounds().contains(mouse)) {
                isPressed = true;

                buttonMarginLight1.setFillColor(theme.getComponentDarkerColor());
                buttonMarginLight2.setFillColor(theme.getComponentDarkerColor());

                buttonPaddingLight1.setFillColor(theme.getComponentDarkColor());
                buttonPaddingLight2.setFillColor(theme.getComponentDarkColor());

                buttonMarginDark1.setFillColor(theme.getComponentLighterColor());
                buttonMarginDark2.setFillColor(theme.getComponentLighterColor());

                buttonPaddingDark1.setFillColor(theme.getComponentLightColor());
                buttonPaddingDark2.setFillColor(theme.getComponentLightColor());

                return true;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left) {

            if (isPressed) {
                isPressed = false;

                buttonMarginLight1.setFillColor(theme.getComponentLighterColor());
                buttonMarginLight2.setFillColor(theme.getComponentLighterColor());

                buttonPaddingLight1.setFillColor(theme.getComponentLightColor());
                buttonPaddingLight2.setFillColor(theme.getComponentLightColor());

                buttonMarginDark1.setFillColor(theme.getComponentDarkerColor());
                buttonMarginDark2.setFillColor(theme.getComponentDarkerColor());

                buttonPaddingDark1.setFillColor(theme.getComponentDarkColor());
                buttonPaddingDark2.setFillColor(theme.getComponentDarkColor());
            }
        }
    }
    return false;
}

