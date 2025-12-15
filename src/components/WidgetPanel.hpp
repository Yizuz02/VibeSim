#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include "../components/InteractiveElement.hpp"
#include "../utils/ColorUtils.hpp"

class WidgetPanel {
private:
    std::pair<int,int> pos;
    std::pair<int,int> size;

    int topPadding = 0;
    int bottomPadding = 0;
    int leftPadding = 0;
    int rightPadding = 0;
    int gap = 0;

    std::vector<InteractiveElement*> interactiveElementList;

    sf::RectangleShape widgetPanelBody;
    sf::RectangleShape widgetPanelBottomMargin;

    sf::Color mainColor;
    sf::Color darkColor;
    sf::Color lightColor;

    // Setup interno
    void setupColors(const sf::Color& main);

public:
    // Constructores
    WidgetPanel(const sf::Color& mainColor,
                std::pair<int,int> size,
                std::pair<int,int> pos,
                int gap,
                int padding);

    WidgetPanel(const sf::Color& mainColor,
                std::pair<int,int> size,
                std::pair<int,int> pos,
                int gap,
                int topPadding,
                int bottomPadding,
                int leftPadding,
                int rightPadding);

    // Padding
    void setTopPadding(int padding);
    void setBottomPadding(int padding);
    void setLeftPadding(int padding);
    void setRightPadding(int padding);

    void setPadding(int top, int bottom, int left, int right);
    void setPadding(int padding);

    // Gap
    void setGap(int gap);

    // Elementos
    void addElement(InteractiveElement& element);

    // Render
    void draw(sf::RenderWindow& window);
};
