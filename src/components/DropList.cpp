#include "DropList.hpp"

void DropList::setupTriangleIcon(std::pair<int,int> size, std::pair<int,int> pos) {
    float radius = size.first/5.0;
    int posx = pos.first+((size.first-(radius*2))/2);
    int posy = pos.second+((size.second-(radius*2))/2) + radius*2;
    triangleIcon.setPointCount(3);
    triangleIcon.setRadius(radius);
    triangleIcon.setPosition(sf::Vector2f(posx, posy));
    triangleIcon.setFillColor(sf::Color::Black);
    triangleIcon.setScale(1.0f, -1.0f);
}

void DropList::setupChoiceBody(std::pair<int,int> size, std::pair<int,int> pos) {
    choiceBody.setSize(sf::Vector2f(size.first, size.second));
    choiceBody.setPosition(sf::Vector2f(pos.first, pos.second));
    choiceBody.setFillColor(theme.getInputColor());
}

void DropList::setupChoicePadding(std::pair<int,int> size, std::pair<int,int> pos) {
    choicePaddingLight1.setSize(sf::Vector2f(2, size.second-2));
    choicePaddingLight1.setPosition(sf::Vector2f(pos.first+size.first-4, pos.second));
    choicePaddingLight1.setFillColor(theme.getComponentLightColor());

    choicePaddingLight2.setSize(sf::Vector2f(size.first-6, 2));
    choicePaddingLight2.setPosition(sf::Vector2f(pos.first+4, pos.second+size.second-4));
    choicePaddingLight2.setFillColor(theme.getComponentLightColor());

    choicePaddingDark1.setSize(sf::Vector2f(2, size.second-4));
    choicePaddingDark1.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    choicePaddingDark1.setFillColor(theme.getComponentDarkerColor());

    choicePaddingDark2.setSize(sf::Vector2f(size.first-6, 2));
    choicePaddingDark2.setPosition(sf::Vector2f(pos.first+2, pos.second+2));
    choicePaddingDark2.setFillColor(theme.getComponentDarkerColor());
}

void DropList::setupChoiceMargins(std::pair<int,int> size, std::pair<int,int> pos) {
    choiceMarginLight1.setSize(sf::Vector2f(2, size.second));
    choiceMarginLight1.setPosition(sf::Vector2f(pos.first+size.first-2, pos.second));
    choiceMarginLight1.setFillColor(theme.getComponentLighterColor());

    choiceMarginLight2.setSize(sf::Vector2f(size.first, 2));
    choiceMarginLight2.setPosition(sf::Vector2f(pos.first, pos.second+size.second-2));
    choiceMarginLight2.setFillColor(theme.getComponentLighterColor());

    choiceMarginDark1.setSize(sf::Vector2f(2, size.second));
    choiceMarginDark1.setPosition(sf::Vector2f(pos.first, pos.second));
    choiceMarginDark1.setFillColor(theme.getComponentDarkColor());

    choiceMarginDark2.setSize(sf::Vector2f(size.first-2, 2));
    choiceMarginDark2.setPosition(sf::Vector2f(pos.first, pos.second));
    choiceMarginDark2.setFillColor(theme.getComponentDarkColor());
}

void DropList::setupChoiceOptionsBodies(std::pair<int,int> size, std::pair<int,int> pos) {
    int posx = pos.first+2;
    int posy = pos.second+2;
    choiceOptionsBodies.clear();
    for (std::string choice: choices) {
        sf::RectangleShape rectangle(sf::Vector2f(size.first-4, size.second));
        rectangle.setFillColor(sf::Color::White);
        rectangle.setPosition(sf::Vector2f(posx, posy));

        sf::Text text;
        text.setFont(theme.getFont());
        text.setString(choice);
        text.setCharacterSize(16);
        text.setFillColor(theme.getFontColor());
        text.setPosition(sf::Vector2f(posx+2, posy+2));

        choiceOptionsBodies.push_back({rectangle,text});
        posy += size.second;
    }

    choiceListBody.setSize(sf::Vector2f(size.first, size.second * choices.size() + 4));
    choiceListBody.setPosition(sf::Vector2f(pos.first, pos.second));
    choiceListBody.setFillColor(theme.getInputColor());
    choiceListBody.setOutlineColor(theme.getComponentDarkerColor());
    choiceListBody.setOutlineThickness(-2);
}

DropList::DropList(std::vector<std::string> choices,
               Theme& theme,
               std::pair<int,int> size,
               std::pair<int,int> pos)
    : InteractiveElement(theme, size, pos, ""),
    choices(choices),
    button("", theme, {size.second-8, size.second-8}, {pos.first+size.first-size.second+4, pos.second+4}) {
        setupChoiceBody(size, pos);
        setupChoicePadding(size, pos);
        setupChoiceMargins(size, pos);
        setupTriangleIcon({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
        setupChoiceOptionsBodies(size,{pos.first, pos.second+size.second});
}

DropList::DropList(std::vector<std::string> choices,
               Theme& theme,
               std::pair<int,int> size)
    : InteractiveElement(theme, size, ""),
    choices(choices),
    button("", theme, {size.second-8, size.second-8}, {pos.first+size.first-size.second+4, pos.second+4}) {
        setupChoiceBody(size, pos);
        setupChoicePadding(size, pos);
        setupChoiceMargins(size, pos);
        setupTriangleIcon({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
        setupChoiceOptionsBodies(size,{pos.first, pos.second+size.second});
}

void DropList::setPosition(std::pair<int,int> pos) {
    this->pos = pos;
    button.setPosition({pos.first+size.first-size.second+4, pos.second+4});
    setupChoiceBody(size, pos);
    setupChoicePadding(size, pos);
    setupChoiceMargins(size, pos);
    setupTriangleIcon({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
    setupChoiceOptionsBodies(size,{pos.first, pos.second+size.second});
}

void DropList::setSize(std::pair<int,int> size) {
    this->size = size;
    button.setSize({size.second-8, size.second-8});
    setupChoiceBody(size, pos);
    setupChoicePadding(size, pos);
    setupChoiceMargins(size, pos);
    setupTriangleIcon({size.second-8, size.second-8},{pos.first+size.first-size.second+4, pos.second+4});
    setupChoiceOptionsBodies(size,{pos.first, pos.second+size.second});
}

void DropList::updateHover(const sf::RenderWindow& window) {
    if (!showChoices) return;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouse(mousePos.x, mousePos.y);

    for (auto& option : choiceOptionsBodies) {
        sf::RectangleShape& rect = option.first;

        if (rect.getGlobalBounds().contains(mouse)) {
            rect.setFillColor(sf::Color(200, 200, 255));
        } else {
            rect.setFillColor(sf::Color::White);
        }
    }
}

void DropList::draw(sf::RenderWindow& window) {  
    window.draw(choiceBody);
    window.draw(choiceMarginDark1);
    window.draw(choiceMarginDark2);
    window.draw(choicePaddingDark1);
    window.draw(choicePaddingDark2);
    window.draw(choiceMarginLight1);
    window.draw(choiceMarginLight2);
    window.draw(choicePaddingLight1);
    window.draw(choicePaddingLight2);
    button.draw(window);
    window.draw(triangleIcon);
    if(showChoices){
        if(!isChoicePressed) {updateHover(window);}
        window.draw(choiceListBody);
        for(std::pair<sf::RectangleShape, sf::Text> choiceObject:choiceOptionsBodies){
            window.draw(choiceObject.first);
            window.draw(choiceObject.second);
        }
        
    }
}

void DropList::setShowChoices(bool showChoices){
    this->showChoices =  showChoices;
}

bool DropList::getShowChoices(){
    return showChoices;
}

bool DropList::isChoiceClicked(sf::Event& event, sf::RenderWindow& window){
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouse(mousePos.x, mousePos.y);

    if (event.mouseButton.button == sf::Mouse::Left && showChoices){
        if (event.type == sf::Event::MouseButtonPressed)
        {   
            for (auto& option : choiceOptionsBodies) {
                sf::RectangleShape& rect = option.first;
                if (rect.getGlobalBounds().contains(mouse)) {
                    rect.setFillColor(sf::Color(150, 150, 255)); 
                    isChoicePressed = true;
                    selected = option.second.getString();
                    return true;
                }
            } 
            if (!button.isClicked(event,window)){
                showChoices = false;
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (isChoicePressed) {
                isChoicePressed = false;
                showChoices = false;
                button.setButtonText(selected);
            }
        }
    } 
    
    return false;
}

bool DropList::isButtonClicked(sf::Event& event, sf::RenderWindow& window) {
    return button.isClicked(event, window);
}