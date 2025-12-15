#include "TitleBar.hpp"

TitleBar::TitleBar(Theme& theme, std::pair<int,int> size)
    : size(size),
      theme(theme),
      closeButton("", theme, {size.second - 4, size.second - 4},
                  {size.first - (size.second + 2), 2}),
      resizeButton("", theme, {size.second - 4, size.second - 4},
                   {size.first - (size.second + 2) * 2, 2}),
      minimizeButton("", theme, {size.second - 4, size.second - 4},
                     {size.first - (size.second + 2) * 3, 2}),
      crossLine1(sf::Vector2f(size.second - 12, 3)),
      crossLine2(sf::Vector2f(size.second - 12, 3)),
      resizeMax(sf::Vector2f(size.second - 16, size.second - 16)),
      resizeMaxTop(sf::Vector2f(size.second - 16, 5))
{
    setupTitleBarBody();

    sf::Color color = sf::Color::Black;

    sf::Vector2f center(
        size.first - (size.second / 2 + 4),
        size.second / 2
    );

    crossLine1.setFillColor(color);
    crossLine1.setOrigin((size.second - 12) / 2, 1.5f);
    crossLine1.setPosition(center);
    crossLine1.setRotation(45.f);

    crossLine2.setFillColor(color);
    crossLine2.setOrigin((size.second - 12) / 2, 1.5f);
    crossLine2.setPosition(center);
    crossLine2.setRotation(-45.f);

    resizeMax.setPosition(
        sf::Vector2f(size.first - (size.second + 2) * 2 + 5, 7));
    resizeMax.setFillColor(sf::Color(0, 0, 0, 0));
    resizeMax.setOutlineThickness(-2);
    resizeMax.setOutlineColor(color);

    resizeMaxTop.setPosition(
        sf::Vector2f(size.first - (size.second + 2) * 2 + 5, 7));
    resizeMaxTop.setFillColor(color);
}


void TitleBar::setupTitleBarBody() {
    titleBarBody.setSize(sf::Vector2f(size.first, size.second));
    titleBarBody.setPosition({0.f, 0.f});
    titleBarBody.setFillColor(theme.getMainColor());
}

void TitleBar::minimize(sf::Window& window) {
    ShowWindow((HWND)window.getSystemHandle(), SW_MINIMIZE);
}

void TitleBar::resize(sf::Window& window) {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    if (!maximized) {
        restoreSize = window.getSize();
        restorePosition = window.getPosition();

        window.setSize({desktop.width, desktop.height});
        window.setPosition({0, 0});
        maximized = true;
    } else {
        window.setSize(restoreSize);
        window.setPosition(restorePosition);
        maximized = false;
    }
}


bool TitleBar::closeButtonPressed(sf::Event& event, sf::RenderWindow& window) {
    return closeButton.isClicked(event, window);
}

bool TitleBar::resizeButtonPressed(sf::Event& event, sf::RenderWindow& window) {
    return resizeButton.isClicked(event, window);
}

bool TitleBar::minimizeButtonPressed(sf::Event& event, sf::RenderWindow& window) {
    return minimizeButton.isClicked(event, window);
}


void TitleBar::handleWindowControls(sf::Event& event, sf::RenderWindow& window) {
    bodyPressed(event, window);

    if (closeButtonPressed(event, window))
        window.close();

    if (resizeButtonPressed(event, window))
        resize(window);

    if (minimizeButtonPressed(event, window))
        minimize(window);
}


void TitleBar::bodyPressed(sf::Event& event, sf::RenderWindow& window) {

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mouseWorld =
            window.mapPixelToCoords(
                {event.mouseButton.x, event.mouseButton.y});

        sf::FloatRect box = titleBarBody.getGlobalBounds();
        box.width = minimizeButton.getPosition().first;

        if (box.contains(mouseWorld)) {
            draggingWindow = true;
            lastMouseScreenPos = sf::Mouse::getPosition();
        }
    }

    if (event.type == sf::Event::MouseMoved && draggingWindow) {

        sf::Vector2i currentMouseScreenPos = sf::Mouse::getPosition();
        sf::Vector2i delta = currentMouseScreenPos - lastMouseScreenPos;

        if (maximized)
            resize(window);

        window.setPosition(window.getPosition() + delta);
        lastMouseScreenPos = currentMouseScreenPos;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {

        draggingWindow = false;
    }
}

void TitleBar::draw(sf::RenderWindow& window) {
    window.draw(titleBarBody);
    minimizeButton.draw(window);
    resizeButton.draw(window);
    closeButton.draw(window);

    window.draw(crossLine1);
    window.draw(crossLine2);
    window.draw(resizeMax);
    window.draw(resizeMaxTop);
}
