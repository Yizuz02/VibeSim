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

long getIndividualAt(std::vector<Individual>& population, float x, float y) {
    for (Individual& ind : population) {
        if (ind.getShape().getGlobalBounds().contains(x, y)) {
            return ind.getId();
        }
    }
    return -1;
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
    DropList inputObstaclePosition({"Center", "Random", "Custom"}, theme, {170,40}, "Position");
    Button buttonRegularObstacle("Add Regular Polygon", theme, {120,40}, " ");
    Button buttonConvexObstacle("Add Convex Shape", theme, {120,40}, " ");
    Button buttonDeleteObstacle("Delete Obstacle", theme, {120,40}, " ");
    Button buttonClearObstacles("Clear Obstacles", theme, {120,40}, " ");

    WidgetPanel obstaclesPanel(theme, panelSize,{0,68}, 5, 5);
    obstaclesPanel.addElement(inputNumSidesObstacle);
    obstaclesPanel.addElement(inputObstaclePosition);
    obstaclesPanel.addElement(buttonRegularObstacle);
    obstaclesPanel.addElement(buttonConvexObstacle);
    obstaclesPanel.addElement(buttonDeleteObstacle);
    obstaclesPanel.addElement(buttonClearObstacles);
    obstaclesPanel.setVisible(false);
    buttonDeleteObstacle.setEnabled(false);
    buttonClearObstacles.setEnabled(false);

    //Creacion del Panel Population
    NumericInput inputPopulationSize(1, 5000, theme, {100,40}, "Population Size");
    NumericInput inputIndividualRadius(1, 30, 10, theme, {100,40}, "Individual Radius");
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
    NumericInput inputStartRadius(1, 200, 30, theme, {100,40}, "Start Radius");
    Button buttonAddStart("Toggle Add Start", theme, {120,40}, " ");
    Button buttonDeleteStart("Delete Start", theme, {120,40}, " ");
    Button buttonClearStarts("Clear Starts", theme, {120,40}, " ");
    NumericInput inputGoalRadius(1, 200, 30, theme, {100,40}, "Goal Radius");
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
    std::map<long,std::pair<int,int>> directions;
    int numThreads = 8;
    //Population populationOriginal = population;
    
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
    ghostInd.setFillColor(theme.getMainLighterColor());

    sf::CircleShape ghostGoal(10.0f);
    

    sf::Cursor cursor;

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
                if (event.mouseWheelScroll.delta > 0)
                    simView.zoom(0.9f);   // zoom in
                else
                    simView.zoom(1.1f);   // zoom out
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
                        float x = static_cast<float>(worldPos.x - r/2);
                        float y = static_cast<float>(worldPos.y - r/2);
                        

                        if (space.contains(x, y, r)) {
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
                        float x = static_cast<float>(worldPos.x);
                        float y = static_cast<float>(worldPos.y);
                        int r = 80;
                        int radius = 80;
                        if (space.contains(x, y, r)) {
                            obstacles.addRegularPolygon({static_cast<int>(x), static_cast<int>(y)},r, inputNumSidesObstacle.getValue());
                            buttonClearObstacles.setEnabled(true);
                        }
                    }
                } 
                if (toggleCreateGoal) {
                    if (event.mouseButton.y>space.getTopBoundary()){
                        int r = inputGoalRadius.getValue();
                        float x = static_cast<float>(worldPos.x - r/2);
                        float y = static_cast<float>(worldPos.y - r/2);
                        
                        int radius = 10.0f;
                        if (space.contains(x, y, r)) {
                            goals.addTarget({static_cast<int>(x), static_cast<int>(y)}, r);
                            buttonClearGoals.setEnabled(true);
                        }
                    }
                } 

                if (!toggleCreateIndividual && !toggleCreateObstacle && !toggleCreateGoal){
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

            inputNumSidesObstacle.handleFocus(event,window);
            inputNumSidesObstacle.handleKeyboardInput(event);
            inputNumSidesObstacle.isDownButtonClicked(event,window);
            inputNumSidesObstacle.isUpButtonClicked(event,window);

            if(inputObstaclePosition.isButtonClicked(event,window)){
                inputObstaclePosition.setShowChoices(!inputObstaclePosition.getShowChoices());
            }
            inputObstaclePosition.isChoiceClicked(event,window);

            if(buttonRegularObstacle.isClicked(event, window)){
                int radius = 80;
                std::pair<int,int> pos;
                if(inputObstaclePosition.getSelected()=="Random"){
                    obstacles.addRegularPolygon(radius, inputNumSidesObstacle.getValue());
                } else if(inputObstaclePosition.getSelected()=="Center"){
                    pos = {space.minX()+(space.getSize().first)/2-radius,space.minY()+(space.getSize().second)/2-radius};
                    obstacles.addRegularPolygon(pos,radius, inputNumSidesObstacle.getValue());
                } else {
                    toggleCreateObstacle = !toggleCreateObstacle;
                    if (toggleCreateObstacle)
                        cursor.loadFromSystem(sf::Cursor::Hand);
                    else
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                    window.setMouseCursor(cursor);
                }
                
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
            
            inputPopulationSize.isDownButtonClicked(event,window);
            inputPopulationSize.isUpButtonClicked(event,window);
            inputPopulationSize.handleFocus(event,window);
            inputPopulationSize.handleKeyboardInput(event);

            inputIndividualRadius.isDownButtonClicked(event,window);
            inputIndividualRadius.isUpButtonClicked(event,window);
            inputIndividualRadius.handleFocus(event,window);
            inputIndividualRadius.handleKeyboardInput(event);

            if (buttonCreatePopulation.isClicked(event,window)){
                populationSize = inputPopulationSize.getValue();
                population.setRadius(inputIndividualRadius.getValue());
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

                for (auto& [id, ind] : population.getIndividuals()) {
                    collisions.addShape(ind->getShape(), false);
                }
                buttonCreatePopulation.setEnabled(false);
                buttonDeletePopulation.setEnabled(true);
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
                selectedIndividual=-1;
                populationSize--;
                buttonDeleteIndividual.setEnabled(false);
                if(populationSize==0){
                    buttonCreatePopulation.setEnabled(true);
                }
            }

            if (buttonDeletePopulation.isClicked(event, window) && populationSize>0) {
                population.clear();
                populationSize=0;
                selectedIndividual = -1;
                buttonCreatePopulation.setEnabled(true);
                buttonDeletePopulation.setEnabled(false);
            }

            // ---------------------------------
            //        GOAL PANEL
            // ---------------------------------

            inputStartRadius.isDownButtonClicked(event,window);
            inputStartRadius.isUpButtonClicked(event,window);
            inputStartRadius.handleFocus(event,window);
            inputStartRadius.handleKeyboardInput(event);

            inputGoalRadius.isDownButtonClicked(event,window);
            inputGoalRadius.isUpButtonClicked(event,window);
            inputGoalRadius.handleFocus(event,window);
            inputGoalRadius.handleKeyboardInput(event);

            if (buttonAddGoal.isClicked(event, window)){
                toggleCreateGoal = !toggleCreateGoal;
                if (toggleCreateGoal)
                    cursor.loadFromSystem(sf::Cursor::Hand);
                else
                    cursor.loadFromSystem(sf::Cursor::Arrow);
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
                ghostInd.setFillColor(theme.getMainLighterColor());
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
        if(start && !pause){
            for (auto& [id, ind] : population.getIndividuals()) {
                std::uniform_real_distribution<float> prob(0.00f, 1.00f);
                if (prob(gen) < 0.0001f) { 
                    int directionx = 0;
                    int directiony = 0;
                    do{
                        directionx = dist(gen);
                        directiony = dist(gen);
                    } while (directionx==0 && directiony==0);
                    directions[id]={directionx,directiony};
                }
                float dx = directions[id].first * speed * dt;
                float dy = directions[id].second * speed * dt;
                if(!ind->move(dx,dy)){
                    int directionx = 0;
                    int directiony = 0;
                    do{
                        directionx = dist(gen);
                        directiony = dist(gen);
                    } while (directionx==0 && directiony==0);
                    directions[id]={directionx,directiony};
                };
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
        obstacles.draw();
        goals.draw();
        population.draw();

        if(toggleCreateIndividual){
            // Posición del mouse relativa a la ventana
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

            // Mover el círculo
            int r = inputIndividualRadius.getValue();
            ghostInd.setPosition(mouseWorld.x - r/2, mouseWorld.y - r/2);
            ghostInd.setRadius(r);
            window.draw(ghostInd);
        }

        if(toggleCreateGoal){
            // Posición del mouse relativa a la ventana
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

            // Mover el círculo
            int r = inputGoalRadius.getValue();
            ghostGoal.setPosition(mouseWorld.x - r/2, mouseWorld.y - r/2);
            ghostGoal.setFillColor(theme.getGoalColor());
            ghostGoal.setRadius(r);
            window.draw(ghostGoal);
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

