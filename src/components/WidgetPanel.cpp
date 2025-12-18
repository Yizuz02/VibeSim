#include "WidgetPanel.hpp"

void WidgetPanel::setupColors(const sf::Color& main) {
    mainColor = main;
    darkColor = ColorUtils::darker(mainColor, 0.5f);
    lightColor = ColorUtils::lighter(mainColor, 1.1f);
}

// --------------------
// Constructores
// --------------------

WidgetPanel::WidgetPanel(const sf::Color& mainColor,
                         std::pair<int,int> size,
                         std::pair<int,int> pos,
                         int gap,
                         int padding)
    : pos(pos), size(size), gap(gap) {

    setPadding(padding);
    setupColors(mainColor);

    widgetPanelBody.setSize(sf::Vector2f(size.first, size.second));
    widgetPanelBody.setPosition(sf::Vector2f(pos.first, pos.second));
    widgetPanelBody.setFillColor(this->mainColor);

    widgetPanelBottomMargin.setSize(sf::Vector2f(size.first, 2));
    widgetPanelBottomMargin.setPosition(
        sf::Vector2f(pos.first, pos.second + size.second - 2)
    );
    widgetPanelBottomMargin.setFillColor(lightColor);
}

WidgetPanel::WidgetPanel(const sf::Color& mainColor,
                         std::pair<int,int> size,
                         std::pair<int,int> pos,
                         int gap,
                         int topPadding,
                         int bottomPadding,
                         int leftPadding,
                         int rightPadding)
    : pos(pos), size(size), gap(gap) {

    setPadding(topPadding, bottomPadding, leftPadding, rightPadding);
    setupColors(mainColor);

    widgetPanelBody.setSize(sf::Vector2f(size.first, size.second));
    widgetPanelBody.setPosition(sf::Vector2f(pos.first, pos.second));
    widgetPanelBody.setFillColor(this->mainColor);

    widgetPanelBottomMargin.setSize(sf::Vector2f(size.first, 2));
    widgetPanelBottomMargin.setPosition(
        sf::Vector2f(pos.first, pos.second + size.second - 2)
    );
    widgetPanelBottomMargin.setFillColor(lightColor);
}

// --------------------
// Padding
// --------------------

void WidgetPanel::setTopPadding(int padding) {
    topPadding = padding;
}

void WidgetPanel::setBottomPadding(int padding) {
    bottomPadding = padding;
}

void WidgetPanel::setLeftPadding(int padding) {
    leftPadding = padding;
}

void WidgetPanel::setRightPadding(int padding) {
    rightPadding = padding;
}

void WidgetPanel::setPadding(int top, int bottom, int left, int right) {
    topPadding = top;
    bottomPadding = bottom;
    leftPadding = left;
    rightPadding = right;
}

void WidgetPanel::setPadding(int padding) {
    topPadding = padding;
    bottomPadding = padding;
    leftPadding = padding;
    rightPadding = padding;
}

// --------------------
// Gap
// --------------------

void WidgetPanel::setGap(int gap) {
    this->gap = gap;
}

void WidgetPanel::setVisible(bool value){
    visible=value;
    for (InteractiveElement* element : interactiveElementList) {
        element->setVisible(value);
    }
}

// --------------------
// Elementos
// --------------------

void WidgetPanel::addElement(InteractiveElement& element) {
    std::pair<int,int> elementPos = {
        pos.first + leftPadding,
        pos.second + topPadding
    };

    for (InteractiveElement* e : interactiveElementList) {
        elementPos.first += e->getSize().first + gap;
    }

    if (element.getSize().second + topPadding + bottomPadding > size.second) {
        auto elementSize = element.getSize();
        element.setSize({
            elementSize.first,
            size.second - topPadding - bottomPadding
        });
    }

    element.setPosition(elementPos);
    interactiveElementList.push_back(&element);
}

// --------------------
// Render
// --------------------

void WidgetPanel::draw(sf::RenderWindow& window) {
    if(visible){
        window.draw(widgetPanelBody);
        window.draw(widgetPanelBottomMargin);

        for (InteractiveElement* element : interactiveElementList) {
            element->draw(window);
        }
    }
}