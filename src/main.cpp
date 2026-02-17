#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_set>
#include <thread>
#include <mutex>
#include "components/Components.hpp"
#include "simulation/Simulation.hpp"
#include <unordered_map>
#include <windows.h>
#include <math.h>


struct Point {
    float x;
    float y;
    float angle = 0.0;
    int parent; 
};

void drawThickLine(
    sf::RenderWindow& window,
    const sf::Vector2f& p1,
    const sf::Vector2f& p2,
    float thickness,
    const sf::Color& color
) {
    sf::Vector2f direction = p2 - p1;
    float length = std::sqrt(direction.x * direction.x +
                             direction.y * direction.y);

    sf::RectangleShape line({ length, thickness });
    line.setPosition(p1);
    line.setFillColor(color);

    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    line.setRotation(angle);

    window.draw(line);
}

std::mt19937 rng(std::random_device{}());
std::uniform_real_distribution<float> fullCircleDist(0.0f, 2.0f * M_PI);
std::uniform_real_distribution<float> thirdCircleDist(-M_PI/4.0f, M_PI/4.0f);

void grow(const Point& startPoint,
          int N,
          float stepSize,
          int totalPaths,
          std::vector<std::pair<Point, Point>>& segments,
          Space& space,
          Targets& goals)
{
    float angle;
    int numChildren = 40;
    bool addChild = true;
    bool addSeg = false;
    std::map<int, std::vector<int>> levels;
    int level = 0;
    levels[level] = {0};
    int numPaths = 0;

    std::vector<Point> nodes;
    nodes.push_back(startPoint);
    nodes[0].parent = -1; 
    while (true){
        std::vector<int> nextLevel;
        for(int parentIndex : levels[level]){
            for(int i=0; i<numChildren; i++){
                addSeg = false;
                if (level==0){
                    angle = fullCircleDist(rng);
                } else {
                    float delta = thirdCircleDist(rng);
                    angle = nodes[parentIndex].angle + delta;
                }
                Point next;
                float nextX = nodes[parentIndex].x;
                float nextY = nodes[parentIndex].y;
                next.x = nextX;
                next.y = nextY;
                for(int j=0; j<stepSize; j++){
                    nextX += std::cos(angle);
                    nextY += std::sin(angle);
                    if(goals.getTargetAt(nextX, nextY)!=-1){
                        nextX += std::cos(angle) * 10;
                        nextY += std::sin(angle) * 10;
                        next.x = nextX;
                        next.y = nextY;
                        addChild = true;
                        addSeg = true;
                        break;
                    }
                    if(space.contains(nextX, nextY) && !space.getCollisions().contains(nextX, nextY)){
                        next.x = nextX;
                        next.y = nextY;
                        addChild = true;
                    } else {
                        addChild = false;
                        break;
                    }
                }
                
                if (addChild){
                    next.angle=angle;
                    next.parent = parentIndex;
                    nodes.push_back(next);
                    int newIndex = nodes.size() - 1;
                    nextLevel.push_back(newIndex);
                    if (addSeg){
                        int idx = newIndex;
                        while (nodes[idx].parent != -1) {
                            int parentIdx = nodes[idx].parent;

                            segments.push_back({
                                nodes[parentIdx],
                                nodes[idx]
                            });

                            idx = parentIdx;
                        }
                        numPaths++;
                        if(numPaths>=totalPaths){
                            return;
                        }
                    }
                } 
            }
        }
        level+=1;
        numChildren = std::max(numChildren/4, N);
        levels[level]=nextLevel;
    }
}

std::vector<std::string> getThemeNames(
    const std::unordered_map<std::string, Theme>& themes)
{
    std::vector<std::string> names;
    names.reserve(themes.size());

    for (const auto& [name, theme] : themes) {
        names.push_back(name);
    }

    return names;
}

float distancePointToSegment(
    const sf::Vector2f& A,
    const sf::Vector2f& B,
    const sf::Vector2f& P)
{
    sf::Vector2f AB = B - A;
    sf::Vector2f AP = P - A;

    float ab2 = AB.x * AB.x + AB.y * AB.y; // |AB|^2
    float dot = AP.x * AB.x + AP.y * AB.y; // AP · AB

    float t = dot / ab2;

    // Clamping al segmento
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;

    sf::Vector2f closest = A + t * AB;

    sf::Vector2f diff = P - closest;

    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}


int main() {

    std::random_device rd;
    std::mt19937 gen(rd());
    int widthWindow=1600;
    int heightWindow=900;
    sf::Vector2i positionWindow;

    std::pair<int, int> panelSize = {1600,50};
    std::pair<int, int> panelMenuSize = {1600,34};
    std::pair<int, int> titleBarSize = {1600,34};
    
    bool toggleMoveView=false;
    bool toggleCreateIndividual = false;
    bool toggleCreateObstacle = false;
    bool toggleCreateConvexObstacle = false;
    bool toggleCreateStart = false;
    bool toggleCreateGoal = false;
    bool isDragging = false;

    bool pause = false;
    bool start = false;

    std::uniform_int_distribution<int> dist(-1, 1);
    sf::RenderWindow window(sf::VideoMode(widthWindow, heightWindow), "VibeSim", sf::Style::None);
    positionWindow = window.getPosition();

    std::unordered_map<std::string, Theme> themes = Theme::loadAllFromFolder("resources/themes");

    Theme theme = themes.at("Classic");

    TitleBar titleBar(theme, titleBarSize);

    //Creacion del Panel Menu
    Button buttonSpace("Space", theme, {90,50});
    Button buttonObstacles("Obstacles", theme, {90,50});
    Button buttonPopulation("Population", theme, {90,50});
    Button buttonTargets("Targets", theme, {90,50});
    Button buttonSimulation("Simulation", theme, {90,50});
    Button buttonAppearance("Appearance", theme, {90,50});
    WidgetPanel menuPanel(theme, panelMenuSize, {0,34}, 1, 2);
    menuPanel.addElement(buttonSpace);
    menuPanel.addElement(buttonObstacles);
    menuPanel.addElement(buttonPopulation);
    menuPanel.addElement(buttonTargets);
    menuPanel.addElement(buttonSimulation);
    menuPanel.addElement(buttonAppearance);

    //Creacion del Panel Space
    NumericInput inputSpaceHeight(200, 10000, 600, theme, {100,40}, "Height");
    NumericInput inputSpaceWidth(200, 10000, 900, theme, {100,40}, "Width");
    Button buttonCreateSpace("Create Space", theme, {120,40}, " ");

    WidgetPanel spacePanel(theme, panelSize,{0,68}, 5, 5);
    spacePanel.addElement(inputSpaceWidth);
    spacePanel.addElement(inputSpaceHeight);
    spacePanel.addElement(buttonCreateSpace);

    //Creacion del Panel Obstacles
    NumericInput inputNumSidesObstacle(3, 100, theme, {100,40}, "Num Sides");
    NumericInput inputObstacleRadius(3, 100, 20, theme, {100,40}, "Radius");
    DropList inputObstaclePosition({"Center", "Random", "Custom"}, theme, {170,40}, "Position");
    Button buttonRegularObstacle("Add Regular Polygon", theme, {120,40}, " ");
    Button buttonConvexObstacle("Add Convex Shape", theme, {120,40}, " ");
    Button buttonDeleteObstacle("Delete Obstacle", theme, {120,40}, " ");
    Button buttonClearObstacles("Clear Obstacles", theme, {120,40}, " ");

    WidgetPanel obstaclesPanel(theme, panelSize,{0,68}, 5, 5);
    obstaclesPanel.addElement(inputNumSidesObstacle);
    obstaclesPanel.addElement(inputObstacleRadius);
    obstaclesPanel.addElement(inputObstaclePosition);
    obstaclesPanel.addElement(buttonRegularObstacle);
    obstaclesPanel.addElement(buttonConvexObstacle);
    obstaclesPanel.addElement(buttonDeleteObstacle);
    obstaclesPanel.addElement(buttonClearObstacles);
    obstaclesPanel.setVisible(false);
    buttonDeleteObstacle.setEnabled(false);
    buttonClearObstacles.setEnabled(false);

    //Creacion del Panel Population
    NumericInput inputPopulationSize(1, 5000, 10, theme, {100,40}, "Population Size");
    NumericInput inputIndividualRadius(1, 30, 5, theme, {100,40}, "Individual Radius");
    Button buttonCreatePopulation("Create Population", theme, {120,40}, " ");
    Button buttonAddIndividual("Toggle Add Individual", theme, {120,40}, " ");
    Button buttonDeleteIndividual("Delete Individual", theme, {120,40}, " ");
    Button buttonDeletePopulation("Delete Population", theme, {120,40}, " ");
    
    WidgetPanel populationPanel(theme, panelSize,{0,68}, 5, 5);
    populationPanel.addElement(inputPopulationSize);
    populationPanel.addElement(inputIndividualRadius);
    populationPanel.addElement(buttonCreatePopulation);
    populationPanel.addElement(buttonAddIndividual);
    populationPanel.addElement(buttonDeleteIndividual);
    populationPanel.addElement(buttonDeletePopulation);
    populationPanel.setVisible(false);
    buttonDeleteIndividual.setEnabled(false);
    buttonDeletePopulation.setEnabled(false);

    // Creación del Panel Targets
    NumericInput inputStartRadius(1, 200, 45, theme, {100,40}, "Start Radius");
    Button buttonAddStart("Toggle Add Start", theme, {120,40}, " ");
    Button buttonDeleteStart("Delete Start", theme, {120,40}, " ");
    Button buttonClearStarts("Clear Starts", theme, {120,40}, " ");
    NumericInput inputGoalRadius(1, 200, 45, theme, {100,40}, "Goal Radius");
    Button buttonAddGoal("Toggle Add Goal", theme, {120,40}, " ");
    Button buttonDeleteGoal("Delete Goal", theme, {120,40}, " ");
    Button buttonClearGoals("Clear Goals", theme, {120,40}, " ");

    WidgetPanel targetsPanel(theme, panelSize, {0,68}, 5, 5);
    targetsPanel.addElement(inputStartRadius);
    targetsPanel.addElement(buttonAddStart);
    targetsPanel.addElement(buttonDeleteStart);
    targetsPanel.addElement(buttonClearStarts);
    targetsPanel.addElement(inputGoalRadius);
    targetsPanel.addElement(buttonAddGoal);
    targetsPanel.addElement(buttonDeleteGoal);
    targetsPanel.addElement(buttonClearGoals);

    targetsPanel.setVisible(false);
    buttonDeleteStart.setEnabled(false);
    buttonClearStarts.setEnabled(false);
    buttonDeleteGoal.setEnabled(false);
    buttonClearGoals.setEnabled(false);

    //Creacion del Panel Simulation
    Button buttonStart("Start", theme, {100,40}, " ");
    Button buttonPause("Pause", theme, {100,40}, " ");
    Button buttonStop("Stop", theme, {100,40}, " ");

    WidgetPanel simulationPanel(theme, panelSize,{0,68}, 5, 5);
    simulationPanel.addElement(buttonStart);
    simulationPanel.addElement(buttonPause);
    simulationPanel.addElement(buttonStop);
    simulationPanel.setVisible(false);
    buttonStop.setEnabled(false);
    buttonPause.setEnabled(false);

    //Creacion del Panel Appearance
    DropList inputThemes(getThemeNames(themes), theme, {170,40}, "Themes");
    Button buttonZoomIn("Zoom In", theme, {80,40}, " ");
    Button buttonZoomOut("Zoom Out", theme, {80,40}, " ");
    Button buttonResetZoom("Reset Zoom", theme, {80,40}, " ");
    Button buttonToggleMoveView("Toggle Move View", theme, {120,40}, " ");
    Button buttonCenterView("Center View", theme, {120,40}, " ");

    WidgetPanel appearancePanel(theme, panelSize,{0,68}, 5, 5);
    appearancePanel.addElement(inputThemes);
    appearancePanel.addElement(buttonZoomIn);
    appearancePanel.addElement(buttonZoomOut);
    appearancePanel.addElement(buttonResetZoom);
    appearancePanel.addElement(buttonToggleMoveView);
    appearancePanel.addElement(buttonCenterView);
    appearancePanel.setVisible(false);
    

    Collisions collisions;
    Space space(panelSize.second+panelMenuSize.second+titleBarSize.second, {900,600}, theme, collisions, window);

    Obstacles obstacles(space, theme);
    Targets starts(space, theme.getStartColor());
    Targets goals (space, theme.getGoalColor());

    Population population(space, theme, 6.f);
    population.setRadius(5.f);
    int populationSize = 0;

    long selectedIndividual = -1;
    long selectedObstacle = -1;
    long selectedGoal = -1;
    long selectedStart = -1;

    std::map<long,std::pair<int,int>> directions;
    
    const float speed = 1; 
    sf::Clock clock;

    sf::View simView;
    simView.setSize(widthWindow, heightWindow);
    simView.setCenter(widthWindow/2, heightWindow/2);
    simView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    sf::View simViewOriginal = simView;

    sf::View hudView;
    hudView.setSize(widthWindow, heightWindow);
    hudView.setCenter(widthWindow/2, heightWindow/2);
    hudView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    sf::Vector2f lastMouseWorldPos;

    sf::CircleShape ghostInd(population.getRadius());

    sf::CircleShape ghostGoal(10.0f);
    sf::CircleShape ghostObs(80.f, 3);

    sf::ConvexShape convex;
    convex.setPointCount(5);
    int pointConvex=0;
    
    sf::Cursor cursor;

    std::map<long, size_t> currentSegmentIndex;

    std::vector<std::pair<Point, Point>> segments;

    while (window.isOpen()) {

        sf::Event event;
        float dt = 0.016f;

        // Obtener posición del mouse
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y},simView);

        // ---------------------------------
        //        MANEJO DE EVENTOS
        // ---------------------------------
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
            
            titleBar.handleWindowControls(event, window);

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (!toggleCreateIndividual && !toggleCreateObstacle && !toggleCreateStart && !toggleCreateGoal){
                    if (event.mouseWheelScroll.delta > 0)
                        simView.zoom(0.9f);   // zoom in
                    else
                        simView.zoom(1.1f);   // zoom out
                } else {
                    int step = 1;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        step = 5;
                    }

                    if (toggleCreateStart) {
                        if (event.mouseWheelScroll.delta > 0)
                            inputStartRadius.setValue(inputStartRadius.getValue() + step);
                        else
                            inputStartRadius.setValue(inputStartRadius.getValue() - step);
                    }

                    if (toggleCreateObstacle) {
                        if (event.mouseWheelScroll.delta > 0)
                            inputObstacleRadius.setValue(inputObstacleRadius.getValue() + step);
                        else
                            inputObstacleRadius.setValue(inputObstacleRadius.getValue() - step);
                    }

                    if (toggleCreateGoal) {
                        if (event.mouseWheelScroll.delta > 0)
                            inputGoalRadius.setValue(inputGoalRadius.getValue() + step);
                        else
                            inputGoalRadius.setValue(inputGoalRadius.getValue() - step);
                    }
                }
                
            }
                
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                isDragging = false;

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f worldPos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}, simView);
                if (toggleMoveView){
                    isDragging = true;
                    lastMouseWorldPos = window.mapPixelToCoords(
                        {event.mouseButton.x, event.mouseButton.y},
                        simView
                    );
                }
                if (toggleCreateIndividual) {
                    if (event.mouseButton.y>space.getTopBoundary()){
                        int r = population.getRadius();
                        float x = static_cast<float>(worldPos.x - r);
                        float y = static_cast<float>(worldPos.y - r);
                        

                        if (space.contains(x+r, y+r, r)) {
                            Individual& newInd =
                                population.createIndividual({static_cast<int>(x), static_cast<int>(y)});
                            populationSize++;
                            buttonCreatePopulation.setEnabled(false);
                            buttonDeletePopulation.setEnabled(true);
                        }
                    }
                } 
                if (toggleCreateObstacle) {
                    if (event.mouseButton.y>space.getTopBoundary()){
                        int r = inputObstacleRadius.getValue();
                        float x = static_cast<float>(worldPos.x - r);
                        float y = static_cast<float>(worldPos.y - r);
                        if (space.contains(x+r, y+r, r)) {
                            obstacles.addRegularPolygon({static_cast<int>(x), static_cast<int>(y)},r, inputNumSidesObstacle.getValue());
                            buttonClearObstacles.setEnabled(true);
                        }
                    }
                } 
                if (toggleCreateGoal) {
                    if (event.mouseButton.y>space.getTopBoundary()){
                        int r = inputGoalRadius.getValue();
                        float x = static_cast<float>(worldPos.x - r);
                        float y = static_cast<float>(worldPos.y - r);

                        if (space.contains(x+r, y+r, r)) {
                            goals.addTarget({static_cast<int>(x), static_cast<int>(y)}, r);
                            segments.clear();
                            buttonClearGoals.setEnabled(true);
                            int N = 3;        // ramificaciones por punto
                            float step = 100;  // tamaño del paso
                            int totalPaths = 1;

                            for (auto& tempStart : starts.getTargets()) {
                                auto* circle = dynamic_cast<sf::CircleShape*>(tempStart.shape.get());

                                float radius = circle->getRadius();

                                float centerX = circle->getPosition().x + radius;
                                float centerY = circle->getPosition().y + radius;

                                Point startPoint{centerX, centerY};

                                grow(startPoint, N, step, totalPaths, segments, space, goals);
                            }
                            
                            
                            std::cout<<"Fin Grow"<<std::endl;
                        }
                    }
                } 
                if (toggleCreateStart) {
                    if (event.mouseButton.y>space.getTopBoundary()){
                        int r = inputStartRadius.getValue();
                        float x = static_cast<float>(worldPos.x - r);
                        float y = static_cast<float>(worldPos.y - r);

                        if (space.contains(x+r, y+r, r)) {
                            starts.addTarget({static_cast<int>(x), static_cast<int>(y)}, r);
                            buttonClearGoals.setEnabled(true);
                        }
                    }
                } 
                if (toggleCreateConvexObstacle){
                    if (event.mouseButton.y>space.getTopBoundary()){
                        float x = static_cast<float>(worldPos.x);
                        float y = static_cast<float>(worldPos.y);

                        if (space.contains(x, y)) {
                            convex.setPoint(pointConvex, sf::Vector2f(x, y));
                            pointConvex++;
                        }
                    }
                    if (pointConvex == inputNumSidesObstacle.getValue()){
                        bool isConvex = true;
                        bool hasPositive = false;
                        bool hasNegative = false;
                        for (size_t i = 0; i < pointConvex; ++i) {

                            const sf::Vector2f& A = convex.getPoint(i);
                            const sf::Vector2f& B = convex.getPoint((i + 1) % pointConvex);
                            const sf::Vector2f& C = convex.getPoint((i + 2) % pointConvex);

                            float cross =
                                (B.x - A.x) * (C.y - B.y) -
                                (B.y - A.y) * (C.x - B.x);

                            if (cross > 0)
                                hasPositive = true;
                            else if (cross < 0)
                                hasNegative = true;

                            if (hasPositive && hasNegative)
                                isConvex = false;
                        }
                        toggleCreateConvexObstacle = false;
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                        window.setMouseCursor(cursor);
                        pointConvex=0;
                        if(isConvex)
                            obstacles.addConvexObstacle(convex);
                        inputNumSidesObstacle.setEnabled(true);
                    }
                }
                if (!toggleCreateIndividual && !toggleCreateObstacle && !toggleCreateStart && !toggleCreateGoal){
                    long clickedInd = population.getIndividualAt(worldPos.x, worldPos.y);
                    if (clickedInd!=-1) {
                        if(selectedIndividual!=-1)
                            population.getIndividual(selectedIndividual)->getShape().setFillColor(theme.getMainColor());
                        if(selectedIndividual==clickedInd){
                            selectedIndividual = -1;
                            buttonDeleteIndividual.setEnabled(false);
                        } else {
                            selectedIndividual = clickedInd;
                            population.getIndividual(selectedIndividual)->getShape().setFillColor(sf::Color::Red);
                            buttonDeleteIndividual.setEnabled(true);
                        }
                    }
                    long clickedObs = obstacles.getObstacleAt(worldPos.x, worldPos.y);
                    if (clickedObs!=-1) {
                        if(selectedObstacle!=-1)
                            obstacles.getObstacle(selectedObstacle)->shape->setFillColor(theme.getSecondaryDarkColor());
                        if(selectedObstacle==clickedObs){
                            selectedObstacle = -1;
                            buttonDeleteObstacle.setEnabled(false);
                        } else {
                            selectedObstacle = clickedObs;
                            obstacles.getObstacle(selectedObstacle)->shape->setFillColor(sf::Color::Red);
                            buttonDeleteObstacle.setEnabled(true);
                        }
                    }
                    long clickedGoal = goals.getTargetAt(worldPos.x, worldPos.y);
                    if (clickedGoal!=-1) {
                        if(selectedGoal!=-1)
                            goals.getTarget(selectedGoal)->shape->setFillColor(theme.getGoalColor());
                        if(selectedGoal==clickedGoal){
                            selectedGoal = -1;
                            buttonDeleteGoal.setEnabled(false);
                        } else {
                            selectedGoal = clickedGoal;
                            goals.getTarget(selectedGoal)->shape->setFillColor(sf::Color::Red);
                            buttonDeleteGoal.setEnabled(true);
                        }
                    }
                    
                    long clickedStart = starts.getTargetAt(worldPos.x, worldPos.y);
                    if (clickedStart!=-1) {
                        if(selectedStart!=-1)
                            starts.getTarget(selectedStart)->shape->setFillColor(theme.getStartColor());
                        if(selectedStart==clickedStart){
                            selectedStart = -1;
                            buttonDeleteStart.setEnabled(false);
                        } else {
                            selectedStart = clickedStart;
                            starts.getTarget(selectedStart)->shape->setFillColor(sf::Color::Red);
                            buttonDeleteStart.setEnabled(true);
                        }
                    }
                }
                
            }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Right)
            {
                if (toggleCreateIndividual || toggleCreateObstacle || toggleCreateStart || toggleCreateGoal || toggleCreateConvexObstacle){
                    toggleCreateIndividual = false;
                    toggleCreateObstacle = false;
                    toggleCreateStart = false;
                    toggleCreateGoal = false;
                    toggleCreateConvexObstacle = false;
                    pointConvex=0;
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                    window.setMouseCursor(cursor);
                    inputNumSidesObstacle.setEnabled(true);
                }
            }
            
            // ---------------------------------
            //        MENU PANEL
            // ---------------------------------

            if(buttonSpace.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(true);
                simulationPanel.setVisible(false);
                appearancePanel.setVisible(false);
                targetsPanel.setVisible(false);
            }
            if(buttonObstacles.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(true);
                spacePanel.setVisible(false);
                simulationPanel.setVisible(false);
                appearancePanel.setVisible(false);
                targetsPanel.setVisible(false);
            }
            if(buttonPopulation.isClicked(event,window)){
                populationPanel.setVisible(true);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                simulationPanel.setVisible(false);
                appearancePanel.setVisible(false);
                targetsPanel.setVisible(false);
            }
            if(buttonSimulation.isClicked(event, window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                simulationPanel.setVisible(true);
                appearancePanel.setVisible(false);
                targetsPanel.setVisible(false);
            }
            if(buttonAppearance.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                simulationPanel.setVisible(false);
                appearancePanel.setVisible(true);
                targetsPanel.setVisible(false);
            }
            if(buttonTargets.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                simulationPanel.setVisible(false);
                appearancePanel.setVisible(false);
                targetsPanel.setVisible(true);
            }
            

            // ---------------------------------
            //        SPACE PANEL
            // ---------------------------------

            inputSpaceWidth.handleFocus(event,window);
            inputSpaceWidth.handleKeyboardInput(event);
            inputSpaceWidth.isDownButtonClicked(event,window);
            inputSpaceWidth.isUpButtonClicked(event,window);

            inputSpaceHeight.handleFocus(event,window);
            inputSpaceHeight.handleKeyboardInput(event);
            inputSpaceHeight.isDownButtonClicked(event,window);
            inputSpaceHeight.isUpButtonClicked(event,window);
            
            if(buttonCreateSpace.isClicked(event,window)){
                population.clear();
                obstacles.clear();
                populationSize=0;
                space.setSize({inputSpaceWidth.getValue(), inputSpaceHeight.getValue()});
            }

            // ---------------------------------
            //        OBSTACLE PANEL
            // ---------------------------------

            inputNumSidesObstacle.handleEvent(event,window);
            inputObstacleRadius.handleEvent(event,window);

            if(inputObstaclePosition.isButtonClicked(event,window)){
                inputObstaclePosition.setShowChoices(!inputObstaclePosition.getShowChoices());
            }
            inputObstaclePosition.isChoiceClicked(event,window);

            if(buttonRegularObstacle.isClicked(event, window)){
                int radius = inputObstacleRadius.getValue();
                int numSides= inputNumSidesObstacle.getValue();
                std::pair<int,int> pos;
                if(inputObstaclePosition.getSelected()=="Random"){
                    obstacles.addRegularPolygon(radius, numSides);
                } else if(inputObstaclePosition.getSelected()=="Center"){
                    pos = {space.minX()+(space.getSize().first)/2-radius,space.minY()+(space.getSize().second)/2-radius};
                    obstacles.addRegularPolygon(pos,radius, numSides);
                } else {
                    toggleCreateObstacle = !toggleCreateObstacle;
                    if (toggleCreateObstacle)
                        cursor.loadFromSystem(sf::Cursor::Hand);
                    else
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                    window.setMouseCursor(cursor);
                }
                
            }

            if(buttonConvexObstacle.isClicked(event, window)){
                toggleCreateConvexObstacle = !toggleCreateConvexObstacle;
                if (toggleCreateConvexObstacle){
                    cursor.loadFromSystem(sf::Cursor::Cross);
                    convex.setPointCount(inputNumSidesObstacle.getValue());
                    inputNumSidesObstacle.setEnabled(false);
                }
                else{
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                    inputNumSidesObstacle.setEnabled(true);
                }
                window.setMouseCursor(cursor);
            }

            if (buttonDeleteObstacle.isClicked(event, window) && selectedObstacle!=-1) {
                obstacles.removeObstacle(selectedObstacle);
                selectedObstacle=-1;
                buttonDeleteObstacle.setEnabled(false);
            }

            if (buttonClearObstacles.isClicked(event, window)) {
                obstacles.clear();
            }

            // ---------------------------------
            //        POPULATION PANEL
            // ---------------------------------
            
            inputPopulationSize.handleEvent(event,window);

            inputIndividualRadius.handleEvent(event,window);

            if (buttonCreatePopulation.isClicked(event,window)){
                populationSize = inputPopulationSize.getValue();
                population.setRadius(inputIndividualRadius.getValue());
                if (starts.empty()){
                    for(long i=0;i<populationSize;i++){
                        Individual ind = population.createIndividual();
                        int directionx = 0;
                        int directiony = 0;
                        do{
                            directionx = dist(gen);
                            directiony = dist(gen);
                        } while (directionx==0 && directiony==0);
                        directions[ind.getId()]={directionx,directiony};
                    }
                } else {
                    std::vector<float> areas;
                    std::vector<float> percentages;
                    float total = 0;
                    areas.reserve(starts.getTargets().size());

                    for (const auto& start : starts.getTargets()) {
                        float r = start.shape->getRadius();
                        float area = static_cast<float>(M_PI) * r * r;
                        areas.push_back(area);
                        total += area;
                    }
                    for (float area : areas){
                        float percentage = area / total;
                        percentages.push_back(percentage);
                    }
                    int indexStart = 0;
                    int popCount = 0;
                    for (float percentage : percentages){
                        const auto& start = starts.getTargets()[indexStart];
                        float r = start.shape->getRadius();
                        sf::Vector2f pos = start.shape->getPosition();
                        for (long i=0; i<populationSize*percentage; i++){
                            if (popCount==populationSize){
                                break;
                            }
                            Individual ind = population.createIndividual({pos.x + r,pos.y + r}, r);
                            int directionx = 0;
                            int directiony = 0;
                            do{
                                directionx = dist(gen);
                                directiony = dist(gen);
                            } while (directionx==0 && directiony==0);
                            directions[ind.getId()]={directionx,directiony};
                            popCount++;
                        }
                        indexStart++;
                    }
                    while (popCount<populationSize){
                        const auto& start = starts.getTargets()[indexStart];
                        float r = start.shape->getRadius();
                        sf::Vector2f pos = start.shape->getPosition();
                        Individual ind = population.createIndividual({pos.x + r,pos.y + r}, r);
                        int directionx = 0;
                        int directiony = 0;
                        do{
                            directionx = dist(gen);
                            directiony = dist(gen);
                        } while (directionx==0 && directiony==0);
                        directions[ind.getId()]={directionx,directiony};
                        popCount++;
                    }
                }
                

                for (auto& [id, ind] : population.getIndividuals()) {
                    collisions.addShape(ind->getShape(), false);
                }
                buttonCreatePopulation.setEnabled(false);
                buttonDeletePopulation.setEnabled(true);
                inputIndividualRadius.setEnabled(false);
                inputPopulationSize.setEnabled(false);
            }

            if (buttonAddIndividual.isClicked(event,window)){
                population.setRadius(inputIndividualRadius.getValue());
                toggleCreateIndividual = !toggleCreateIndividual;
                if (toggleCreateIndividual)
                    cursor.loadFromSystem(sf::Cursor::Hand);
                else
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                window.setMouseCursor(cursor);
            }

            if (buttonDeleteIndividual.isClicked(event, window) && selectedIndividual!=-1) {
                population.removeIndividual(selectedIndividual);
                directions.erase(selectedIndividual);
                currentSegmentIndex.erase(selectedIndividual);
                selectedIndividual=-1;
                populationSize--;
                buttonDeleteIndividual.setEnabled(false);
                if(populationSize==0){
                    buttonCreatePopulation.setEnabled(true);
                    inputIndividualRadius.setEnabled(true);
                    inputPopulationSize.setEnabled(true);
                }
            }

            if (buttonDeletePopulation.isClicked(event, window) && populationSize>0) {
                population.clear();
                currentSegmentIndex.clear();
                populationSize=0;
                selectedIndividual = -1;
                buttonCreatePopulation.setEnabled(true);
                buttonDeletePopulation.setEnabled(false);
                inputIndividualRadius.setEnabled(true);
                inputPopulationSize.setEnabled(true);
            }

            // ---------------------------------
            //        GOAL PANEL
            // ---------------------------------

            inputStartRadius.handleEvent(event,window);
            inputGoalRadius.handleEvent(event,window);

            if (buttonAddStart.isClicked(event, window)){
                toggleCreateStart = !toggleCreateStart;
                if (toggleCreateStart){
                    cursor.loadFromSystem(sf::Cursor::Hand);
                }
                else{
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                }
                window.setMouseCursor(cursor);
            }

            if (buttonDeleteStart.isClicked(event, window) && selectedStart!=-1) {
                starts.removeTarget(selectedStart);
                selectedStart=-1;
                buttonDeleteStart.setEnabled(false);
            }

            if (buttonAddGoal.isClicked(event, window)){
                toggleCreateGoal = !toggleCreateGoal;
                if (toggleCreateGoal){
                    cursor.loadFromSystem(sf::Cursor::Hand);
                }
                else{
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                }
                window.setMouseCursor(cursor);
            }

            if (buttonDeleteGoal.isClicked(event, window) && selectedGoal!=-1) {
                goals.removeTarget(selectedGoal);
                selectedGoal=-1;
                buttonDeleteGoal.setEnabled(false);
            }

            // ---------------------------------
            //        SIMULATION PANEL
            // ---------------------------------

            if (buttonStart.isClicked(event, window)){
                start = true;
                buttonStart.setEnabled(false);
                buttonStop.setEnabled(true);
                buttonPause.setEnabled(true);
            }

            if (buttonPause.isClicked(event, window)){
                pause = !pause;
                if(pause){
                    buttonPause.setButtonText("Resume");
                } else {
                    buttonPause.setButtonText("Pause");
                }
            }

            if (buttonStop.isClicked(event, window)){
                start = false;
                buttonStart.setEnabled(true);
                buttonStop.setEnabled(false);
                buttonPause.setEnabled(false);
            }


            // ---------------------------------
            //        APPEARANCE PANEL
            // ---------------------------------

            if(inputThemes.isButtonClicked(event,window)){
                inputThemes.setShowChoices(!inputThemes.getShowChoices());
            }
            if(inputThemes.isChoiceClicked(event,window)){
                Theme& newTheme = themes.at(inputThemes.getSelected());
                titleBar.setTheme(newTheme);
                menuPanel.setTheme(newTheme);
                spacePanel.setTheme(newTheme);
                populationPanel.setTheme(newTheme);
                obstaclesPanel.setTheme(newTheme);
                targetsPanel.setTheme(newTheme);
                simulationPanel.setTheme(newTheme);
                appearancePanel.setTheme(newTheme);
                population.setTheme(newTheme);
                space.setTheme(newTheme);
                obstacles.setTheme(newTheme);
                goals.setColor(newTheme.getGoalColor());
                starts.setColor(newTheme.getStartColor());
            }
            if (buttonZoomIn.isClicked(event, window)){
                simView.zoom(0.9f);
            }
            if (buttonZoomOut.isClicked(event, window)){
                simView.zoom(1.1f);   // zoom out
            }
            if (buttonResetZoom.isClicked(event, window)){
                simView.setSize(simViewOriginal.getSize());
            }
            if (buttonToggleMoveView.isClicked(event, window)){
                toggleMoveView = !toggleMoveView;
                if (toggleMoveView)
                    cursor.loadFromSystem(sf::Cursor::Hand);
                else
                    cursor.loadFromSystem(sf::Cursor::Arrow);
                window.setMouseCursor(cursor);
            }
            if (buttonCenterView.isClicked(event, window)){
                simView.setCenter(simViewOriginal.getCenter());
            }
            
        }

        // ---------------------------------
        //       MOVIMIENTO CONTINUO
        // ---------------------------------
        if (start && !pause) {
            for (auto& [id, ind] : population.getIndividuals()) {
                bool pass=false;
                sf::Vector2f topLeft1 = ind->getShape().getPosition();  
                float r1 = ind->getShape().getRadius();
                sf::Vector2f c1 = {
                    topLeft1.x + r1,
                    topLeft1.y + r1
                };
                for(const auto& goal: goals.getTargets()){
                    sf::Vector2f topLeft2 = goal.shape->getPosition(); 
                    float r2 = goal.shape->getRadius();
                    sf::Vector2f c2 = {
                        topLeft2.x + r2,
                        topLeft2.y + r2
                    };

                    float dx = c1.x - c2.x;
                    float dy = c1.y - c2.y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    if(distance + r1 <= r2){
                        pass=true;
                    }
                }
                if(pass)
                    continue;
                
                // Si hay segmentos disponibles, usar el más cercano
                if (!segments.empty()) {
                    // Obtener índice actual del segmento
                    size_t segIdx;

                    if (!currentSegmentIndex.count(id)) {
                        // PRIMER FRAME → buscar el segmento más cercano

                        float minDist = std::numeric_limits<float>::max();

                        for (size_t i = 0; i < segments.size(); ++i) {
                            
                            const auto& seg = segments[i];
                            sf::Vector2f A(seg.first.x, seg.first.y);
                            sf::Vector2f B(seg.second.x, seg.second.y);
                            float distStart = distancePointToSegment(A, B, ind->getShape().getPosition());

                            if (distStart < minDist) {
                                minDist = distStart;
                                segIdx = i;
                            }
                        }

                        currentSegmentIndex[id] = segIdx;  // inicializar
                    }
                    else {
                        segIdx = currentSegmentIndex[id];  // ya estaba inicializado
                    }
                    // Tomar el segmento actual
                    const auto& seg = segments[segIdx];

                    // Vector hacia el final del segmento
                    float dx = seg.second.x - ind->getShape().getPosition().x;
                    float dy = seg.second.y - ind->getShape().getPosition().y;
                    float length = std::sqrt(dx*dx + dy*dy);

                    if (length > 0.0f) {
                        dx = (dx / length) * speed * dt;
                        dy = (dy / length) * speed * dt;

                        // Intentar mover
                        if (ind->move(dx, dy)) {
                            // Si alcanzó el final del segmento, pasar al siguiente
                            float distToEnd = std::sqrt(
                                (seg.second.x - ind->getShape().getPosition().x)*(seg.second.x - ind->getShape().getPosition().x) +
                                (seg.second.y - ind->getShape().getPosition().y)*(seg.second.y - ind->getShape().getPosition().y)
                            );
                            if (distToEnd < speed * dt * 1.5f) { // tolerancia
                                currentSegmentIndex[id] = std::min(segIdx + 1, segments.size() - 1);
                            }
                        } else {
                            // Si no puede moverse, buscar otro segmento cercano
                            // (opcional: podrías recorrer todos los segmentos y cambiar al que permita mover)
                        }
                    }
                } else {
                    // Movimiento aleatorio si no hay segmentos
                    std::uniform_real_distribution<float> prob(0.00f, 1.00f);
                    if (prob(gen) < 0.0001f) { 
                        int directionx = 0;
                        int directiony = 0;
                        do {
                            directionx = dist(gen);
                            directiony = dist(gen);
                        } while (directionx == 0 && directiony == 0);
                        directions[id] = {directionx, directiony};
                    }

                    float dx = directions[id].first * speed * dt;
                    float dy = directions[id].second * speed * dt;

                    if (!ind->move(dx, dy)) {
                        int directionx = 0;
                        int directiony = 0;
                        do {
                            directionx = dist(gen);
                            directiony = dist(gen);
                        } while (directionx == 0 && directiony == 0);
                        directions[id] = {directionx, directiony};
                    }
                }

            }
        }
     
        if (window.hasFocus()) {  // evita movimiento si la ventana no está activa
            if (isDragging){
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f currentMouseWorldPos =
                    window.mapPixelToCoords(pixelPos, simView);

                sf::Vector2f delta = lastMouseWorldPos - currentMouseWorldPos;

                simView.move(delta);

                lastMouseWorldPos = window.mapPixelToCoords(
                    sf::Mouse::getPosition(window),
                    simView
                );
            }
        }

        // ---------------------------------
        //              RENDER
        // ---------------------------------
        window.clear(theme.getBackgroundColor());   
        window.setView(simView); 
        space.draw(); 
        goals.draw();
        starts.draw();
        
        for (const auto& s : segments) {
            drawThickLine(window, sf::Vector2f(s.first.x, s.first.y), sf::Vector2f(s.second.x, s.second.y), 1, theme.getSecondaryLightColor());
        }

        population.draw();
        obstacles.draw();
        

        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
        if(toggleCreateIndividual){
            int r = population.getRadius();
            ghostInd.setPosition(mouseWorld.x - r, mouseWorld.y - r);
            sf::Color c = theme.getMainLighterColor();
            c.a = 80; 
            ghostInd.setFillColor(c);
            ghostInd.setRadius(r);
            window.draw(ghostInd);
        }

        if(toggleCreateGoal){
            int r = inputGoalRadius.getValue();
            ghostGoal.setPosition(mouseWorld.x - r, mouseWorld.y - r);
            sf::Color c = theme.getGoalColor();
            c.a = 80; 
            ghostGoal.setFillColor(c);
            ghostGoal.setRadius(r);
            window.draw(ghostGoal);
        }

        if(toggleCreateStart){
            int r = inputStartRadius.getValue();
            ghostGoal.setPosition(mouseWorld.x - r, mouseWorld.y - r);
            sf::Color c = theme.getStartColor();
            c.a = 80; 
            ghostGoal.setFillColor(c);
            ghostGoal.setRadius(r);
            window.draw(ghostGoal);
        }
        if(toggleCreateObstacle){
            int r = inputObstacleRadius.getValue();
            ghostObs.setPointCount(inputNumSidesObstacle.getValue());
            ghostObs.setPosition(mouseWorld.x - r, mouseWorld.y - r);
            sf::Color c = theme.getSecondaryColor();
            c.a = 80; 
            ghostObs.setFillColor(c);
            ghostObs.setRadius(r);
            window.draw(ghostObs);
        }
        if(toggleCreateConvexObstacle){
            sf::Color c = theme.getSecondaryColor();
            c.a = 80; 
            float thickness = 4.f;
            if(pointConvex>=1){
                sf::Vector2f p1(convex.getPoint(pointConvex-1));
                sf::Vector2f p2(mouseWorld.x, mouseWorld.y);
                drawThickLine(window, p1, p2, thickness, c);
            }
            if(pointConvex>=2){
                for(int i=0;i<pointConvex-1;i++){
                    sf::Vector2f p1(convex.getPoint(i));
                    sf::Vector2f p2(convex.getPoint(i+1));
                    drawThickLine(window, p1, p2, thickness, c);
                }
                
            }
        }
        

        window.setView(hudView); 
        menuPanel.draw(window);
        spacePanel.draw(window);
        populationPanel.draw(window);
        obstaclesPanel.draw(window);
        simulationPanel.draw(window);
        appearancePanel.draw(window);
        targetsPanel.draw(window);
        titleBar.draw(window);
        window.display();
    }

    return 0;
}

