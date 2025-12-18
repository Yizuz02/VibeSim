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

Individual createIndividual(long id, Space& space, Theme& theme){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disty(space.minY(), space.maxY());
    std::uniform_int_distribution<int> distx(space.minX(), space.maxX());
    int posx = distx(gen);
    int posy = disty(gen);
    return Individual(id, 5.0, {posx, posy}, space, theme);
}

long getIndividualAt(std::vector<Individual>& population, float x, float y) {
    for (Individual& ind : population) {
        if (ind.getShape().getGlobalBounds().contains(x, y)) {
            return ind.getId();
        }
    }
    return -1;
}


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    int widthWindow=1600;
    int heightWindow=900;
    sf::Vector2i positionWindow;

    sf::Font font;
    font.loadFromFile("resources/fonts/W95FA.otf");

    std::uniform_int_distribution<int> dist(-1, 1);
    sf::RenderWindow window(sf::VideoMode(widthWindow, heightWindow), "Simulador 2D", sf::Style::None);
    positionWindow = window.getPosition();

    Theme themeW95(sf::Color(190, 190, 190), sf::Color(8, 39, 245), sf::Color(255, 255, 0), sf::Color::White, sf::Color::Black, font);

    TitleBar titleBar(themeW95, {1600, 34});

    //Creacion del Panel Menu
    Button buttonSpace("Space", themeW95, {90,50});
    Button buttonObstacles("Obstacles", themeW95, {90,50});
    Button buttonPopulation("Population", themeW95, {90,50});
    Button buttonGoals("Goals", themeW95, {90,50});
    Button buttonAppearance("Appearance", themeW95, {90,50});
    WidgetPanel menuPanel(sf::Color(190, 190, 190), {1600,34}, {0,34}, 1, 2);
    menuPanel.addElement(buttonSpace);
    menuPanel.addElement(buttonObstacles);
    menuPanel.addElement(buttonPopulation);
    menuPanel.addElement(buttonGoals);
    menuPanel.addElement(buttonAppearance);

    //Creacion del Panel Space
    NumericInput inputSpaceHeight(200, 900, 600, themeW95, {100,40});
    NumericInput inputSpaceWidth(200, 1600, 900, themeW95, {100,40});
    Button buttonCreateSpace("Create Space", themeW95, {120,40});

    WidgetPanel spacePanel(sf::Color(190, 190, 190), {1600,44},{0,68}, 5, 5);
    spacePanel.addElement(inputSpaceWidth);
    spacePanel.addElement(inputSpaceHeight);
    spacePanel.addElement(buttonCreateSpace);

    //Creacion del Panel Obstacles
    NumericInput inputNumSidesObstacle(3, 100, themeW95, {100,40});
    DropList inputObstaclePosition({"Center", "Random", "Custom"}, themeW95, {170,40});
    Button buttonRegularObstacle("Add Regular Polygon", themeW95, {120,40});
    Button buttonConvexObstacle("Add Convex Shape", themeW95, {120,40});
    Button buttonDeleteObstacle("Delete Obstacle", themeW95, {120,40});
    Button buttonClearObstacles("Clear Obstacles", themeW95, {120,40});

    WidgetPanel obstaclesPanel(sf::Color(190, 190, 190), {1600,44},{0,68}, 5, 5);
    obstaclesPanel.addElement(inputNumSidesObstacle);
    obstaclesPanel.addElement(inputObstaclePosition);
    obstaclesPanel.addElement(buttonRegularObstacle);
    obstaclesPanel.addElement(buttonConvexObstacle);
    obstaclesPanel.addElement(buttonDeleteObstacle);
    obstaclesPanel.addElement(buttonClearObstacles);
    obstaclesPanel.setVisible(false);

    //Creacion del Panel Population
    NumericInput inputPopulationSize(1, 5000, themeW95, {100,40});
    Button buttonCreatePopulation("Create Population", themeW95, {120,40});
    Button buttonAddIndividual("Add Individual", themeW95, {120,40});
    Button buttonDeleteIndividual("Delete Individual", themeW95, {120,40});
    Button buttonDeletePopulation("Delete Population", themeW95, {120,40});
    
    WidgetPanel populationPanel(sf::Color(190, 190, 190), {1600,44},{0,68}, 5, 5);
    populationPanel.addElement(inputPopulationSize);
    populationPanel.addElement(buttonCreatePopulation);
    populationPanel.addElement(buttonAddIndividual);
    populationPanel.addElement(buttonDeleteIndividual);
    populationPanel.addElement(buttonDeletePopulation);
    populationPanel.setVisible(false);

    //Creacion del Panel Appearance
    DropList inputThemes({"Windows 95", "Coral", "Rose"}, themeW95, {170,40});
    Button buttonApplyTheme("Apply Theme", themeW95, {120,40});

    WidgetPanel appearancePanel(sf::Color(190, 190, 190), {1600,44},{0,68}, 5, 5);
    appearancePanel.addElement(inputThemes);
    appearancePanel.addElement(buttonApplyTheme);
    appearancePanel.setVisible(false);
    

    Collisions collisions;
    Space space(112, {900,600}, sf::Color(1, 130, 129), collisions, window);

    Obstacles obstacles(space, themeW95);

    Population population(space, themeW95, 6.f);
    population.setRadius(5.f);
    int populationSize = 0;
    long selectedIndividual = -1;
    long selectedObstacle = -1;
    std::map<long,std::pair<int,int>> directions;
    int numThreads = 8;
    
    const float speed = 1; 
    sf::Clock clock;

    while (window.isOpen()) {

        sf::Event event;
        float dt = 0.016f;

        // Obtener posición del mouse
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));


        // ---------------------------------
        //        MANEJO DE EVENTOS
        // ---------------------------------
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
            
            titleBar.handleWindowControls(event, window);


            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                long clickedInd = population.getIndividualAt(mouse.x, mouse.y);
                if (clickedInd!=-1) {
                    if(selectedIndividual!=-1)
                        population.getIndividual(selectedIndividual)->getShape().setFillColor(themeW95.getMainColor());
                    if(selectedIndividual==clickedInd){
                        selectedIndividual = -1;
                    } else {
                        selectedIndividual = clickedInd;
                        population.getIndividual(selectedIndividual)->getShape().setFillColor(sf::Color::Red);
                    }
                }
                long clickedObs = obstacles.getObstacleAt(mouse.x, mouse.y);
                if (clickedObs!=-1) {
                    if(selectedObstacle!=-1)
                        obstacles.getObstacle(selectedObstacle)->shape->setFillColor(themeW95.getSecondaryColor());
                    if(selectedObstacle==clickedObs){
                        selectedObstacle = -1;
                    } else {
                        selectedObstacle = clickedObs;
                        obstacles.getObstacle(selectedObstacle)->shape->setFillColor(sf::Color::Red);
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
                appearancePanel.setVisible(false);
            }
            if(buttonObstacles.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(true);
                spacePanel.setVisible(false);
                appearancePanel.setVisible(false);
            }
            if(buttonPopulation.isClicked(event,window)){
                populationPanel.setVisible(true);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                appearancePanel.setVisible(false);
            }
            if(buttonAppearance.isClicked(event,window)){
                populationPanel.setVisible(false);
                obstaclesPanel.setVisible(false);
                spacePanel.setVisible(false);
                appearancePanel.setVisible(true);
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
                } else {
                    pos = {space.minX()+(space.getSize().first)/2-radius,space.minY()+(space.getSize().second)/2-radius};
                    obstacles.addRegularPolygon(pos,radius, inputNumSidesObstacle.getValue());
                }
                
            }

            if (buttonDeleteObstacle.isClicked(event, window) && selectedObstacle!=-1) {
                obstacles.removeObstacle(selectedObstacle);
                selectedObstacle=-1;
            }

            if (buttonClearObstacles.isClicked(event, window)) {
                population.clear();
            }

            // ---------------------------------
            //        POPULATION PANEL
            // ---------------------------------
            
            inputPopulationSize.isDownButtonClicked(event,window);
            inputPopulationSize.isUpButtonClicked(event,window);
            inputPopulationSize.handleFocus(event,window);
            inputPopulationSize.handleKeyboardInput(event);

            if (buttonCreatePopulation.isClicked(event,window) && populationSize==0){
                populationSize = inputPopulationSize.getValue();
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
            }

            if (buttonAddIndividual.isClicked(event,window)){
                Individual newInd = population.createIndividual();
                collisions.addShape(newInd.getShape(), false);
                populationSize++;
            }

            if (buttonDeleteIndividual.isClicked(event, window) && selectedIndividual!=-1) {
                population.removeIndividual(selectedIndividual);
                directions.erase(selectedIndividual);
                selectedIndividual=-1;
                populationSize--;
            }

            if (buttonDeletePopulation.isClicked(event, window) && populationSize>0) {
                population.clear();
                populationSize=0;
            }

            // ---------------------------------
            //        APPEARANCE PANEL
            // ---------------------------------

            if(inputThemes.isButtonClicked(event,window)){
                inputThemes.setShowChoices(!inputThemes.getShowChoices());
            }
            if(inputThemes.isChoiceClicked(event,window)){

            }
            
        }

        // ---------------------------------
        //       MOVIMIENTO CONTINUO
        // ---------------------------------
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
            
     
        if (window.hasFocus()) {  // evita movimiento si la ventana no está activa
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                std::cout<<"Hola\n";
        }

        // ---------------------------------
        //              RENDER
        // ---------------------------------
        window.clear(sf::Color::White);    
        space.draw(); 
        obstacles.draw();
        population.draw();
        menuPanel.draw(window);
        spacePanel.draw(window);
        populationPanel.draw(window);
        obstaclesPanel.draw(window);
        appearancePanel.draw(window);
        titleBar.draw(window);
        window.display();
    }

    return 0;
}

