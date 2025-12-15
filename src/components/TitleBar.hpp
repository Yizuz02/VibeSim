#pragma once

#include <SFML/Graphics.hpp>
#include <utility>
#include <windows.h>

#include "../styles/Theme.hpp"
#include "Button.hpp"

class TitleBar {
private:
    std::pair<int,int> size;

    sf::Vector2i lastMouseScreenPos;
    bool draggingWindow = false;

    Theme& theme;

    Button minimizeButton;
    Button resizeButton;
    Button closeButton;

    sf::Vector2u restoreSize;
    sf::Vector2i restorePosition;
    bool maximized = false;

    sf::RectangleShape titleBarBody;
    sf::RectangleShape crossLine1;
    sf::RectangleShape crossLine2;
    sf::RectangleShape resizeMax;
    sf::RectangleShape resizeMaxTop;

    void setupTitleBarBody();
    void minimize(sf::Window& window);
    void resize(sf::Window& window);

public:
    TitleBar(Theme& theme, std::pair<int,int> size);

    bool closeButtonPressed(sf::Event& event, sf::RenderWindow& window);
    bool resizeButtonPressed(sf::Event& event, sf::RenderWindow& window);
    bool minimizeButtonPressed(sf::Event& event, sf::RenderWindow& window);

    void handleWindowControls(sf::Event& event, sf::RenderWindow& window);
    void bodyPressed(sf::Event& event, sf::RenderWindow& window);

    void draw(sf::RenderWindow& window);
};
