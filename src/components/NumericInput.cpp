#include "NumericInput.hpp"

void NumericInput::setupTrianglesIcons(std::pair<int,int> size, std::pair<int,int> pos) {
    float radius = size.first/6.0;
    int posx = pos.first+((size.first-(radius*2))/2);
    int posy = pos.second+((size.second-(radius*2))/2) + radius*2;
    triangleIconDown.setPointCount(3);
    triangleIconDown.setRadius(radius);
    triangleIconDown.setPosition(sf::Vector2f(posx, posy+radius));
    triangleIconDown.setFillColor(sf::Color::Black);
    triangleIconDown.setScale(1.0f, -1.0f);
    triangleIconUp.setPointCount(3);
    triangleIconUp.setRadius(radius);
    triangleIconUp.setPosition(sf::Vector2f(posx, posy-(radius*3.5)));
    triangleIconUp.setFillColor(sf::Color::Black);
    triangleIconUp.setScale(1.0f, 1.0f);
}

void NumericInput::setupInputBody() {
    inputBody.setSize(sf::Vector2f(size.first, size.second));
    inputBody.setPosition(sf::Vector2f(pos.first, pos.second));
    inputBody.setFillColor(theme.getInputColor());
}

void NumericInput::setupInputPadding() {
    inputPaddingLight1.setSize(sf::Vector2f(2, size.second-2));
    inputPaddingLight1.setPosition(sf::Vector2f(pos.first+size.first-4, pos.second));
    inputPaddingLight1.setFillColor(theme.getComponentLightColor());

    inputPaddingLight2.setSize(sf::Vector2f(size.first-6, 2));
    inputPaddingLight2.setPosition(sf::Vector2f(pos.first+4, pos.second+size.second-4));
    inputPaddingLight2.setFillColor(theme.getComponentLightColor());

    inputPaddingDark1.setSize(sf::Vector2f(2, size.second-4));
    inputPaddingDark1.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    inputPaddingDark1.setFillColor(theme.getComponentDarkerColor());

    inputPaddingDark2.setSize(sf::Vector2f(size.first-6, 2));
    inputPaddingDark2.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    inputPaddingDark2.setFillColor(theme.getComponentDarkerColor());
}

void NumericInput::setupInputMargins() {
    inputMarginLight1.setSize(sf::Vector2f(2, size.second));
    inputMarginLight1.setPosition(sf::Vector2f(pos.first+size.first-2, pos.second));
    inputMarginLight1.setFillColor(theme.getComponentLighterColor());

    inputMarginLight2.setSize(sf::Vector2f(size.first, 2));
    inputMarginLight2.setPosition(sf::Vector2f(pos.first, pos.second+size.second-2));
    inputMarginLight2.setFillColor(theme.getComponentLighterColor());

    inputMarginDark1.setSize(sf::Vector2f(2, size.second));
    inputMarginDark1.setPosition(sf::Vector2f(pos.first, pos.second));
    inputMarginDark1.setFillColor(theme.getComponentDarkColor());

    inputMarginDark2.setSize(sf::Vector2f(size.first-2, 2));
    inputMarginDark2.setPosition(sf::Vector2f(pos.first, pos.second));
    inputMarginDark2.setFillColor(theme.getComponentDarkColor());
}

void NumericInput::setupValueText() {
    int fontSize = size.second/2.5;
    valueText.setFont(theme.getFont());
    valueText.setString(std::to_string(value));
    valueText.setCharacterSize(fontSize);
    valueText.setPosition(sf::Vector2f(pos.first+8, pos.second+((size.second-valueText.getGlobalBounds().height)/3)));
    valueText.setFillColor(theme.getFontColor());
}

NumericInput::NumericInput(int minValue, 
               int maxValue,
               Theme& theme,
               std::pair<int,int> size,
               std::pair<int,int> pos)
    : InteractiveElement(theme, size, pos, ""),
    minValue(minValue),
    maxValue(maxValue),
    value(minValue),
    buffer(std::to_string(value)),
    upButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4}),
    downButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4+(size.second-8)/2}) {
        setupInputBody();
        setupInputPadding();
        setupInputMargins();
        setupTrianglesIcons({size.second-8, (size.second-8)/2},{pos.first+size.first-size.second+4, pos.second+4});
        setupValueText();
}

NumericInput::NumericInput(int minValue, 
               int maxValue,
               int value,
               Theme& theme,
               std::pair<int,int> size,
               std::pair<int,int> pos)
    : InteractiveElement(theme, size, pos, ""),
    minValue(minValue),
    maxValue(maxValue),
    value(value),
    buffer(std::to_string(value)),
    upButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4}),
    downButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4+(size.second-8)/2}) {
        setupInputBody();
        setupInputPadding();
        setupInputMargins();
        setupTrianglesIcons({size.second-8, (size.second-8)/2},{pos.first+size.first-size.second+4, pos.second+4});
        setupValueText();
}

NumericInput::NumericInput(int minValue, 
               int maxValue,
               Theme& theme,
               std::pair<int,int> size)
    : InteractiveElement(theme, size, ""),
    minValue(minValue),
    maxValue(maxValue),
    value(minValue),
    buffer(std::to_string(value)),
    upButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4}),
    downButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4+(size.second-8)/2}) {
        setupInputBody();
        setupInputPadding();
        setupInputMargins();
        setupTrianglesIcons({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
        setupValueText();
}


NumericInput::NumericInput(int minValue, 
               int maxValue,
               int value,
               Theme& theme,
               std::pair<int,int> size)
    : InteractiveElement(theme, size, ""),
    minValue(minValue),
    maxValue(maxValue),
    value(value),
    buffer(std::to_string(value)),
    upButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4}),
    downButton("", theme, {size.second-8, (size.second-8)/2}, {pos.first+size.first-size.second+4, pos.second+4+(size.second-8)/2}) {
        setupInputBody();
        setupInputPadding();
        setupInputMargins();
        setupTrianglesIcons({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
        setupValueText();
}

void NumericInput::setPosition(std::pair<int,int> pos) {
    this->pos = pos;
    upButton.setPosition({pos.first+size.first-size.second+4, pos.second+4});
    downButton.setPosition({pos.first+size.first-size.second+4, pos.second+4+(size.second-8)/2});
    setupInputBody();
    setupInputPadding();
    setupInputMargins();
    setupTrianglesIcons({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
    setupValueText();
}

void NumericInput::setSize(std::pair<int,int> size) {
    this->size = size;
    upButton.setSize({size.second-8, (size.second-8)/2});
    downButton.setSize({size.second-8, (size.second-8)/2});
    setupInputBody();
    setupInputPadding();
    setupInputMargins();
    setupTrianglesIcons({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
    setupValueText();
}

int NumericInput::getValue(){
    return value;
}


void NumericInput::draw(sf::RenderWindow& window) {  
    if (visible){
        window.draw(inputBody);
        window.draw(inputMarginDark1);
        window.draw(inputMarginDark2);
        window.draw(inputPaddingDark1);
        window.draw(inputPaddingDark2);
        window.draw(inputMarginLight1);
        window.draw(inputMarginLight2);
        window.draw(inputPaddingLight1);
        window.draw(inputPaddingLight2);
        window.draw(valueText);
        upButton.draw(window);
        downButton.draw(window);
        window.draw(triangleIconDown);
        window.draw(triangleIconUp);
    }
}


bool NumericInput::isUpButtonClicked(sf::Event& event, sf::RenderWindow& window) {
    if(enabled){
        if(upButton.isClicked(event, window)){
            if(value<maxValue){
                value++;
                buffer=std::to_string(value);
                setupValueText();
                return true;
            }
            
        }
    }
    return false;
}

bool NumericInput::isDownButtonClicked(sf::Event& event, sf::RenderWindow& window) {
    if(enabled){
        if(downButton.isClicked(event, window)){
            if(value>minValue){
                value--;
                buffer=std::to_string(value);
                setupValueText();
                return true;
            }
            
        }
    }
    return false;
}

bool NumericInput::handleFocus(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mouse =
            window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

        if (inputBody.getGlobalBounds().contains(mouse)) {
            focused = true;
            return true;
        } else {
            focused = false;
        }
    }
    return false;
}


void NumericInput::handleKeyboardInput(sf::Event& event) {
    if (!focused || !enabled){
        int newValue = std::stoi(buffer);
        if (newValue < minValue) {
            value = minValue;
        } else if (newValue > maxValue){
            value = maxValue;
        } else {
            value = buffer.empty() ? minValue : std::stoi(buffer);
        }
        setupValueText();
        buffer=std::to_string(value);
        return;
    } 

    // Entrada de texto (números)
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= '0' && event.text.unicode <= '9') {
            buffer += static_cast<char>(event.text.unicode);
            valueText.setString(buffer);
        }
    }

    // Teclas especiales
    if (event.type == sf::Event::KeyPressed) {

        // Borrar
        if (event.key.code == sf::Keyboard::Backspace && !buffer.empty()) {
            buffer.pop_back();
            valueText.setString(buffer);
        }

        // Flechas
        if (event.key.code == sf::Keyboard::Up && value < maxValue) {
            value++;
            buffer = std::to_string(value);
            setupValueText();
        }

        if (event.key.code == sf::Keyboard::Down && value > minValue) {
            value--;
            buffer = std::to_string(value);
            setupValueText();
        }

        // Enter → confirmar y salir del foco
        if (event.key.code == sf::Keyboard::Enter) {
            focused = false;
        }
    }
}
