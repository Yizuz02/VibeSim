#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <utility>
#include "../styles/Theme.hpp"

class InteractiveElement {
protected:
    std::pair<int, int> pos;
    std::pair<int, int> size;

    // Estado
    bool visible = true;
    bool enabled = true;

    // Tema
    Theme& theme;

    // Texto
    sf::Text labelText;

    // Setup interno
    void setupLabelText(std::string& text, std::pair<int,int> pos);

public:
    // Constructores
    InteractiveElement(Theme& theme,
                       std::pair<int,int> size,
                       std::pair<int,int> pos,
                       std::string labelText);

    InteractiveElement(Theme& theme,
                       std::pair<int,int> size,
                       std::string labelText);

    // Destructor virtual
    virtual ~InteractiveElement() = default;

    // Setters
    virtual void setPosition(std::pair<int,int> pos);
    virtual void setSize(std::pair<int,int> size);
    void setLabelText(const std::string& text);

    void setVisible(bool value);
    void setEnabled(bool value);

    // Getters
    std::pair<int,int> getPosition() const;
    std::pair<int,int> getSize() const;
    std::string getLabelText() const;

    bool isVisible() const;
    bool isEnabled() const;

    // Render
    virtual void draw(sf::RenderWindow& window) = 0;
};
