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
#ifdef _WIN32
#include <windows.h>
#endif
#include <math.h>
#include <cstdlib>
#include <string>
#include <exception>


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
          Targets& goals,
          int r)
{
    float angle;
    int numChildren = 40;
    bool addChild = true;
    bool addSeg = false;
    std::map<int, std::vector<int>> levels;
    int level = 0;
    levels[level] = {0};
    int numPaths = 0;
    sf::FloatRect box;

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
                    box = sf::FloatRect(
                        static_cast<float>(nextX),
                        static_cast<float>(nextY),
                        r*2.0f,
                        r*2.0f
                    );
                    int target = goals.getTargetAt(nextX, nextY);
                    if(target!=-1){
                        nextX += std::cos(angle) * r * 3;
                        nextY += std::sin(angle) * r * 3;
                        next.x = nextX;
                        next.y = nextY;
                        addChild = true;
                        addSeg = true;
                        break;
                    }
                    if(space.contains(box) && !space.getCollisions().contains(box)){
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
                    // Salvaguarda: limita la exploracion para evitar bucles
                    // infinitos cuando la meta es inalcanzable (el arbol
                    // crece de forma exponencial por nivel).
                    if (nodes.size() % 5000 == 0)
                        std::cout << "Grow: explorando... nodos=" << nodes.size() << std::endl;
                    if (nodes.size() >= 20000) {
                        std::cout << "Grow: limite de exploracion alcanzado ("
                                  << nodes.size() << " nodos)" << std::endl;
                        return;
                    }
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
        if (level > 30) {
            std::cout << "Grow: profundidad maxima alcanzada (" << level
                      << " niveles)" << std::endl;
            return;
        }
        if (nextLevel.empty()) {
            std::cout << "Grow: sin ramas viables en el nivel " << level
                      << std::endl;
            return;
        }
        // Limita el numero de puntas activas por nivel: sin este control el
        // arbol crece en amplitud de forma exponencial y el tope de nodos
        // corta la exploracion en profundidad (metas lejanas inalcanzables).
        // Se conservan hasta 60 puntas muestreadas uniformemente, lo que
        // permite explorar en profundidad como un micelio real.
        if (nextLevel.size() > 60) {
            std::vector<int> sampled;
            sampled.reserve(60);
            for (int k = 0; k < 60; ++k)
                sampled.push_back(
                    nextLevel[(static_cast<size_t>(k) * nextLevel.size()) / 60]);
            nextLevel = std::move(sampled);
        }
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


int main(int argc, char* argv[]) {

    // ------------------------------------------------------------------
    // Modo AUTOTEST (instrumentacion para pruebas de rendimiento).
    // Uso: ./VibeSim --autotest [--n <individuos>] [--seconds <duracion>]
    //                 [--out <prefijo_de_archivos>]
    // Se activa unicamente con --autotest; el uso normal no se afecta.
    // ------------------------------------------------------------------
    bool autotest = false;
    int autoN = 2000;
    float autoSeconds = 15.f;
    std::string autoPrefix = "autotest";
    for (int a = 1; a < argc; ++a) {
        std::string arg = argv[a];
        if (arg == "--autotest") autotest = true;
        else if (arg == "--n" && a + 1 < argc) autoN = std::max(1, std::atoi(argv[++a]));
        else if (arg == "--seconds" && a + 1 < argc) autoSeconds = std::max(1.f, static_cast<float>(std::atof(argv[++a])));
        else if (arg == "--out" && a + 1 < argc) autoPrefix = argv[++a];
    }

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

    // ------------------------------------------------------------------
    // Instrumentacion del modo AUTOTEST (solo afecta con --autotest).
    // ------------------------------------------------------------------
    sf::Clock moveClock;
    sf::Clock autoClock;
    sf::Clock evClock;
    sf::Clock rdClock;
    double autoEvMs = 0.0;
    double autoRdMs = 0.0;
    float autoWindowTime = 0.f;
    float autoReportTime = 0.f;
    unsigned long long autoReportFrames = 0;
    unsigned long long autoTotalFrames = 0;
    double autoMoveMs = 0.0;
    double autoFpsSum = 0.0;
    int autoFpsSamples = 0;
    double autoMinFps = 1e9;
    int autoPhase = 0;      // 0: captura inicial -> 1: simular -> 2: terminar
    bool autoMidShot = false;
    long autoPlaced = 0;

    auto captureFrame = [&](const std::string& path) {
        sf::Texture tex;
        if (tex.create(window.getSize().x, window.getSize().y)) {
            tex.update(window);
            tex.copyToImage().saveToFile(path);
            std::cout << "[autotest] captura guardada: " << path << std::endl;
        } else {
            std::cout << "[autotest] ERROR: no se pudo capturar " << path << std::endl;
        }
    };

    if (autotest) {
        try {
            window.setPosition(sf::Vector2i(40, 40));
            space.setSize({1100, 750});
            std::cout << "[autotest] espacio redimensionado a 1100x750" << std::endl;

            // Obstaculo central: obliga a desviar la red sin bloquearla.
            obstacles.addRegularPolygon({space.minX() + 350.f, space.minY() + 305.f}, 80.f, 6);

            // Radio del individuo segun el tamano de poblacion solicitado.
            float indRadius = autoN <= 700 ? 5.f : (autoN <= 2200 ? 3.f : 2.f);
            population.setRadius(indRadius);
            float cell = (1.8f * indRadius) * (1.8f * indRadius) * 2.0f;
            float needArea = autoN * cell;
            float zoneR = std::max(60.f, std::min(150.f,
                std::sqrt(needArea / (2.f * 3.14159265f))));
            std::cout << "[autotest] radio individuo=" << indRadius
                      << " radio zonas=" << zoneR << std::endl;

            // Zonas de despliegue (inicio) apiladas a la izquierda y meta a la derecha.
            starts.addTarget({space.minX() + 200.f - zoneR, space.minY() + 240.f - zoneR}, zoneR);
            starts.addTarget({space.minX() + 200.f - zoneR, space.minY() + 540.f - zoneR}, zoneR);
            goals.addTarget({space.minX() + 600.f, space.minY() + 235.f}, 150.f);
            std::cout << "[autotest] zonas=" << starts.getTargets().size()
                      << " metas=" << goals.getTargets().size() << std::endl;

            // Generacion de la red de rutas (mismo flujo que la interfaz).
            segments.clear();
            for (auto& tempStart : starts.getTargets()) {
                auto* circle = dynamic_cast<sf::CircleShape*>(tempStart.shape.get());
                float radius = circle->getRadius();
                float centerX = circle->getPosition().x + radius;
                float centerY = circle->getPosition().y + radius;
                Point startPoint{centerX, centerY};
                grow(startPoint, 3, 60, 6, segments, space, goals,
                     static_cast<int>(population.getRadius()));
            }
            std::cout << "[autotest] red de rutas generada: "
                      << segments.size() << " segmentos" << std::endl;

            // Colocacion de la poblacion dentro de las zonas de inicio.
            for (int i = 0; i < autoN; ++i) {
                const auto& zone = starts.getTargets()[i % starts.getTargets().size()];
                float r = zone.shape->getRadius();
                sf::Vector2f pos = zone.shape->getPosition();
                try {
                    Individual& ind = population.createIndividual(
                        {static_cast<int>(pos.x + r), static_cast<int>(pos.y + r)},
                        static_cast<int>(r));
                    int directionx = 0;
                    int directiony = 0;
                    do {
                        directionx = dist(gen);
                        directiony = dist(gen);
                    } while (directionx == 0 && directiony == 0);
                    directions[ind.getId()] = {directionx, directiony};
                    autoPlaced++;
                } catch (const std::exception&) {
                    break; // zona saturada: se detiene la colocacion
                }
            }
            std::cout << "[autotest] poblacion colocada: " << autoPlaced
                      << "/" << autoN << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[autotest] ERROR en el escenario: " << e.what() << std::endl;
            window.close();
        }
    }

    // Descarta el tiempo de preparacion del escenario de la medicion de FPS.
    if (autotest) autoClock.restart();

    while (window.isOpen()) {

        sf::Event event;
        float dt = 0.016f;

        // Obtener posición del mouse
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y},simView);

        // ---------------------------------
        //        MANEJO DE EVENTOS
        // ---------------------------------
        evClock.restart();
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
                            int totalPaths = 50;

                            for (auto& tempStart : starts.getTargets()) {
                                auto* circle = dynamic_cast<sf::CircleShape*>(tempStart.shape.get());

                                float radius = circle->getRadius();

                                float centerX = circle->getPosition().x + radius;
                                float centerY = circle->getPosition().y + radius;

                                Point startPoint{centerX, centerY};

                                grow(startPoint, N, step, totalPaths, segments, space, goals, inputIndividualRadius.getValue());
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
                            std::cout << ind.getId() << std::endl;
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
                if(starts.empty()){
                    segments.clear();
                }
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
                if(goals.empty()){
                    segments.clear();
                }
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
        if (autotest) autoEvMs += evClock.getElapsedTime().asMilliseconds();
        if (start && !pause) {
            moveClock.restart();
            for (auto& [id, ind] : population.getIndividuals()) {
                sf::Vector2f topLeft1 = ind->getShape().getPosition();  
                float r1 = ind->getShape().getRadius();
                                sf::Vector2f c1 = {
                                    topLeft1.x + r1,
                                    topLeft1.y + r1
                                };bool insideGoal = false;
                sf::Vector2f goalCenter;
                float goalRadius = 0.f;

                for(const auto& goal : goals.getTargets()) {

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
                        insideGoal = true;
                        goalCenter = c2;
                        goalRadius = r2;
                        break;
                    }
                }

                if(insideGoal) {

                    std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.14159265f);
                    std::uniform_real_distribution<float> radiusDist(0.f, goalRadius - r1);

                    float angle = angleDist(gen);
                    float rad   = radiusDist(gen);

                    float targetX = goalCenter.x + std::cos(angle) * rad;
                    float targetY = goalCenter.y + std::sin(angle) * rad;

                    // convertir a top-left (porque tu shape usa posición como esquina)
                    float newLeft = targetX - r1;
                    float newTop  = targetY - r1;

                    sf::Vector2f current = ind->getShape().getPosition();
                    float dx = newLeft - current.x;
                    float dy = newTop  - current.y;

                    // mover suavemente hacia ese punto
                    float length = std::sqrt(dx*dx + dy*dy);
                    if(length > 0.f){
                        dx = (dx / length) * speed * dt;
                        dy = (dy / length) * speed * dt;
                        ind->move(dx, dy);
                    }

                    continue;
                }


                
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
                        if (!autotest)
                            std::cout <<"Id: "<< id << " Seg: " << currentSegmentIndex[id] << std::endl;
                    }
                    else {
                        segIdx = currentSegmentIndex[id];  // ya estaba inicializado
                    }
                    // Tomar el segmento actual
                    const auto& seg = segments[segIdx];
                    sf::Vector2f P = ind->getShape().getPosition();
                    sf::Vector2f A(seg.first.x, seg.first.y);
                    sf::Vector2f B(seg.second.x, seg.second.y);

                    sf::Vector2f AB = B - A;
                    sf::Vector2f AP = P - A;

                    float ab2 = AB.x * AB.x + AB.y * AB.y;

                    // Manejar segmento degenerado
                    sf::Vector2f closest;
                    if (ab2 == 0.f) {
                        closest = A;
                    } else {
                        float t = (AP.x * AB.x + AP.y * AB.y) / ab2;

                        // Clamp al segmento
                        if (t < 0.f) t = 0.f;
                        if (t > 1.f) t = 1.f;

                        closest = A + t * AB;
                    }

                    // Vector hacia el punto más cercano del segmento
                    float dx = closest.x - P.x;
                    float dy = closest.y - P.y;

                    float length = std::sqrt(dx * dx + dy * dy);

                    if (length < 0.2f) {
                        dx = seg.second.x - ind->getShape().getPosition().x;
                        dy = seg.second.y - ind->getShape().getPosition().y;
                        length = std::sqrt(dx*dx + dy*dy);
                        dx = (dx / length) * speed * dt;
                        dy = (dy / length) * speed * dt;
                        // Intentar mover
                        const int maxAttempts = 18;
                        bool moved = false;

                        for (int attempt = 0; attempt < maxAttempts; ++attempt) {

                            float factor = 1.0f - (attempt * 0.05f);  // 1.0, 0.8, 0.6, 0.4, 0.2
                            float tryDx = dx * factor;
                            float tryDy = dy * factor;

                            if (ind->move(tryDx, tryDy)) {
                                moved = true;
                                break;
                            }
                        }
                        if (moved){
                            float distToEnd = std::sqrt(
                            (seg.second.x - ind->getShape().getPosition().x)*(seg.second.x - ind->getShape().getPosition().x) +
                            (seg.second.y - ind->getShape().getPosition().y)*(seg.second.y - ind->getShape().getPosition().y)
                            );
                            if (distToEnd < speed * dt * 1.5f) { // tolerancia
                            currentSegmentIndex.erase(id);
                            }
                        }
                    } else {
                        dx = (dx / length) * speed * dt;
                        dy = (dy / length) * speed * dt;
                        const int maxAttempts = 18;
                        bool moved = false;

                        for (int attempt = 0; attempt < maxAttempts; ++attempt) {

                            float factor = 1.0f - (attempt * 0.05f);  // 1.0, 0.8, 0.6, 0.4, 0.2
                            float tryDx = dx * factor;
                            float tryDy = dy * factor;

                            if (ind->move(tryDx, tryDy)) {
                                moved = true;
                                break;
                            }
                        }
                        if (!moved){
                            dx = seg.second.x - ind->getShape().getPosition().x;
                            dy = seg.second.y - ind->getShape().getPosition().y;
                            length = std::sqrt(dx*dx + dy*dy);
                            dx = (dx / length) * speed * dt;
                            dy = (dy / length) * speed * dt;
                            // Intentar mover
                            const int maxAttempts = 18;
                            bool moved = false;

                            for (int attempt = 0; attempt < maxAttempts; ++attempt) {

                                float factor = 1.0f - (attempt * 0.05f);  // 1.0, 0.8, 0.6, 0.4, 0.2
                                float tryDx = dx * factor;
                                float tryDy = dy * factor;

                                if (ind->move(tryDx, tryDy)) {
                                    moved = true;
                                    break;
                                }
                            }

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
            if (autotest) autoMoveMs += moveClock.getElapsedTime().asMilliseconds();
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
        rdClock.restart();
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
        if (autotest) autoRdMs += rdClock.getElapsedTime().asMilliseconds();

        // ---- Instrumentacion del modo autotest (reporte y capturas) ----
        if (autotest) {
            float elapsed = autoClock.restart().asSeconds();
            if (autoTotalFrames <= 3)
                std::cout << "[autotest] frame=" << autoTotalFrames
                          << " elapsed=" << elapsed << "s" << std::endl;
            autoWindowTime += elapsed;
            autoReportTime += elapsed;
            autoReportFrames++;
            autoTotalFrames++;

            if (autoPhase == 0) {
                // Primer fotograma: escenario completo, aun sin movimiento.
                captureFrame(autoPrefix + "_0_escenario.png");
                start = true; // arranca la simulacion
                autoPhase = 1;
            }

            if (autoPhase == 1 && autoWindowTime >= 2.0f && !autoMidShot) {
                captureFrame(autoPrefix + "_1_simulacion.png");
                autoMidShot = true;
            }

            if (autoReportTime >= 1.0f) {
                double fps = autoReportFrames / autoReportTime;
                autoFpsSum += fps;
                autoFpsSamples++;
                autoMinFps = std::min(autoMinFps, fps);
                std::cout << "[autotest] t=" << autoWindowTime
                          << "s fps=" << fps
                          << " poblacion=" << population.size()
                          << " segmentos=" << segments.size() << std::endl;
                autoReportTime = 0.f;
                autoReportFrames = 0;
            }

            if (autoWindowTime >= autoSeconds) {
                double avgFps = autoFpsSamples ? autoFpsSum / autoFpsSamples : 0.0;
                double avgMoveMs = autoTotalFrames ? autoMoveMs / autoTotalFrames : 0.0;
                std::cout << "[autotest] RESULTADO: poblacion=" << population.size()
                          << " frames=" << autoTotalFrames
                          << " fps_prom=" << avgFps
                          << " fps_min=" << (autoFpsSamples ? autoMinFps : 0.0)
                          << " ms_actualizacion_prom=" << avgMoveMs
                          << " ms_eventos_prom=" << (autoTotalFrames ? autoEvMs / autoTotalFrames : 0.0)
                          << " ms_render_prom=" << (autoTotalFrames ? autoRdMs / autoTotalFrames : 0.0)
                          << std::endl;
                captureFrame(autoPrefix + "_2_final.png");
                window.close();
            }
        }
    }

    return 0;
}

