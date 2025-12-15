#pragma once
#include "Button.hpp"
#include "InteractiveElement.hpp"
#include <vector>
#include <string>

class DropList : public InteractiveElement {
protected:
    std::vector<std::string> choices;
    std::string selected;
    bool showChoices = false;
    bool isChoicePressed = false;

    Button button;

    sf::RectangleShape choiceBody;
    sf::RectangleShape choiceListBody;
    std::vector<std::pair<sf::RectangleShape, sf::Text>> choiceOptionsBodies;
    sf::CircleShape triangleIcon;

    sf::RectangleShape choiceMarginDark1;
    sf::RectangleShape choiceMarginDark2;
    sf::RectangleShape choiceMarginLight1;
    sf::RectangleShape choiceMarginLight2;
    sf::RectangleShape choicePaddingDark1;
    sf::RectangleShape choicePaddingDark2;
    sf::RectangleShape choicePaddingLight1;
    sf::RectangleShape choicePaddingLight2;

    void setupTriangleIcon(std::pair<int,int> size, std::pair<int,int> pos);
    void setupChoiceBody(std::pair<int,int> size, std::pair<int,int> pos);
    void setupChoicePadding(std::pair<int,int> size, std::pair<int,int> pos);
    void setupChoiceMargins(std::pair<int,int> size, std::pair<int,int> pos);
    void setupChoiceOptionsBodies(std::pair<int,int> size, std::pair<int,int> pos);

public:
    DropList(std::vector<std::string> choices, Theme& theme, std::pair<int,int> size);
    DropList(std::vector<std::string> choices, Theme& theme, std::pair<int,int> size, std::pair<int,int> pos);

    void setPosition(std::pair<int,int> pos) override;
    void setSize(std::pair<int,int> size) override;
    void setShowChoices(bool showChoices);

    bool getShowChoices();

    void updateHover(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) override;

    bool isChoiceClicked(sf::Event& event, sf::RenderWindow& window);
    bool isButtonClicked(sf::Event& event, sf::RenderWindow& window);
};
