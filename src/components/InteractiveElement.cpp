#include "InteractiveElement.hpp"

// Constructor con posición
InteractiveElement::InteractiveElement(Theme& theme,
                                       std::pair<int,int> size,
                                       std::pair<int,int> pos,
                                       std::string labelText)
    : pos(pos),
      size(size),
      theme(theme) {

    setupLabelText(pos);
    setLabelText(labelText);
}

// Constructor sin posición (por defecto 0,0)
InteractiveElement::InteractiveElement(Theme& theme,
                                       std::pair<int,int> size,
                                       std::string labelText)
    : pos({0, 0}),
      size(size),
      theme(theme) {

    setupLabelText(pos);
    setLabelText(labelText);
}

// Setup del texto
void InteractiveElement::setupLabelText(std::pair<int,int> pos) {
    int fontSize = size.second/2.8;

    labelText.setFont(theme.getFont());
    labelText.setFillColor(theme.getFontColor());
    labelText.setCharacterSize(fontSize);

    labelText.setPosition(
        static_cast<float>(pos.first + 2),
        static_cast<float>(pos.second - 12)
    );
}

// Setters

void InteractiveElement::setPosition(std::pair<int,int> newPos) {
    pos = newPos;
    labelText.setPosition(
        static_cast<float>(pos.first),
        static_cast<float>(pos.second)
    );
}

void InteractiveElement::setSize(std::pair<int,int> newSize) {
    size = newSize;

    labelText.setCharacterSize(static_cast<unsigned int>(size.second * 0.5f));
}

void InteractiveElement::setLabelText(const std::string& newText) {
    labelText.setString(newText);
}

void InteractiveElement::setTheme(Theme &theme){
    this->theme = theme;
}

void InteractiveElement::setVisible(bool value) {
    visible = value;
}

void InteractiveElement::setEnabled(bool value) {
    enabled = value;
}


// Getters

std::pair<int,int> InteractiveElement::getPosition() const {
    return pos;
}

std::pair<int,int> InteractiveElement::getSize() const {
    return size;
}

std::string InteractiveElement::getLabelText() const {
    return labelText.getString();
}

bool InteractiveElement::isVisible() const {
    return visible;
}

bool InteractiveElement::isEnabled() const {
    return enabled;
}
