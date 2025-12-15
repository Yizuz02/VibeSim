#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <unordered_set>
#include <thread>
#include <mutex>
#include "components/Button.hpp"
#include "components/DropList.hpp"
#include "components/WidgetPanel.hpp"
#include "components/TitleBar.hpp"
#include <unordered_map>
#include <windows.h>

class Individual{
    private:
        long id;
        std::pair<int,int> pos;
        float radius;
        sf::Color mainColor;
        sf::CircleShape individualBody;

    public:
        Individual(long id, sf::Color mainColor, float radius, std::pair<int,int> pos){
            this->id = id;
            this->pos = pos;
            this->radius = radius;
            individualBody.setRadius(radius);
            individualBody.setPosition(sf::Vector2f(pos.first, pos.second));
            individualBody.setFillColor(mainColor);
        }

        void draw(sf::RenderWindow& window){  
            window.draw(individualBody);
        }


        bool move(std::unordered_set<uint64_t>& obstacles, float dx, float dy, float minX, float minY, float maxX, float maxY)
        {   
            if(dy==0 && dx==0) return true;
            sf::FloatRect next = individualBody.getGlobalBounds();

            int oldLeft = std::floor(next.left);
            int oldRight = std::floor(next.left + next.width  - 1);
            int oldTop = std::floor(next.top);
            int oldBottom = std::floor(next.top + next.height - 1);

            next.top += dy;
            next.left += dx;

            int left = std::floor(next.left);
            int right = std::floor(next.left + next.width  - 1);
            int top = std::floor(next.top);
            int bottom = std::floor(next.top + next.height - 1);

            // --- LÍMITES DEL MUNDO ---
            if (left < minX || top < minY) return false;
            if (right > maxX || bottom > maxY) return false;

            // --- COLISIÓN ---
            if (dy > 0) {
                for (int x = left; x <= right; x++) {
                    uint64_t key = (uint64_t(x) << 32) | uint32_t(bottom+1);
                    if (obstacles.count(key)) return false;
                }
            }
            else if (dy < 0) {
                for (int x = left; x <= right; x++) {
                    uint64_t key = (uint64_t(x) << 32) | uint32_t(top-1);
                    if (obstacles.count(key)) return false;
                }
            }

            if (dx > 0) {
                for (int y = top; y <= bottom; y++) {
                    uint64_t key = (uint64_t(right+1) << 32) | uint32_t(y);
                    if (obstacles.count(key)) return false;
                }
            }
            else if (dx < 0) {
                for (int y = top; y <= bottom; y++) {
                    uint64_t key = (uint64_t(left-1) << 32) | uint32_t(y);
                    if (obstacles.count(key)) return false;
                }
            }
            if(oldBottom!=bottom){
                for (int x = oldLeft; x <= oldRight+1; x++){
                    obstacles.erase(((uint64_t)x << 32) | (uint32_t)oldBottom);
                    obstacles.erase(((uint64_t)x << 32) | (uint32_t)oldBottom+1);
                    obstacles.erase(((uint64_t)x << 32) | (uint32_t)oldTop);
                    obstacles.erase(((uint64_t)x << 32) | (uint32_t)oldTop-1);
                }
                for (int x = left; x <= right; x++){
                    obstacles.insert(((uint64_t)x << 32) | (uint32_t)bottom);
                    obstacles.insert(((uint64_t)x << 32) | (uint32_t)top);
                }
            }

            
            if(oldLeft!=left){
                for (int y = oldTop; y <= oldBottom; y++){
                    obstacles.erase(((uint64_t)oldLeft << 32) | (uint32_t)y);
                    obstacles.erase(((uint64_t)oldLeft-1 << 32) | (uint32_t)y);
                    obstacles.erase(((uint64_t)oldRight << 32) | (uint32_t)y);
                    obstacles.erase(((uint64_t)oldRight+1 << 32) | (uint32_t)y);
                }
                for (int y = top; y <= bottom; y++){
                    obstacles.insert(((uint64_t)left << 32) | (uint32_t)y);
                    obstacles.insert(((uint64_t)right << 32) | (uint32_t)y);
                }
            }

            individualBody.move(dx, dy);
            return true;
        }

        long getId() const {
            return id;
        }

        sf::CircleShape& getShape(){
            return individualBody;
        }

};

Individual createIndividual(long id){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disty(100, 800);
    std::uniform_int_distribution<int> distx(100, 1500);
    int posx = distx(gen);
    int posy = disty(gen);
    return Individual(id, sf::Color(8,39,245), 5.0, {posx, posy});
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
    Button buttonPopulation("Population", themeW95, {90,50});
    Button buttonObstacles("Obstacles", themeW95, {90,50});
    Button buttonGoals("Goals", themeW95, {90,50});
    WidgetPanel menuPanel(sf::Color(190, 190, 190), {1600,34}, {0,34}, 1, 2);
    menuPanel.addElement(buttonSpace);
    menuPanel.addElement(buttonPopulation);
    menuPanel.addElement(buttonObstacles);
    menuPanel.addElement(buttonGoals);
    
    std::unordered_map<std::string, WidgetPanel> panelMap;
    Button buttonCreatePopulation("Create Population", themeW95, {120,40});
    Button buttonAddIndividual("Add individual", themeW95, {120,40});
    Button buttonDeleteIndividual("Delete individual", themeW95, {120,40});
    Button buttonDeletepPopulation("Delete Population", themeW95, {120,40});
    DropList button3({"Hola", "Adios", "AAAAA"}, themeW95, {170,40});
    
    WidgetPanel populationPanel(sf::Color(190, 190, 190), {1600,45},{0,68}, 5, 5);
    populationPanel.addElement(buttonCreatePopulation);
    populationPanel.addElement(buttonAddIndividual);
    populationPanel.addElement(buttonDeleteIndividual);
    populationPanel.addElement(buttonDeletepPopulation);

    std::vector<Individual> population;
    long selectedIndividual = -1;
    std::map<long,std::pair<int,int>> directions;
    int numThreads = 8;
    
    long lastId;
    for(long i=0;i<200;i++){
        population.push_back(createIndividual(i));
        int directionx = 0;
        int directiony = 0;
        do{
            directionx = dist(gen);
            directiony = dist(gen);
        } while (directionx==0 && directiony==0);
        directions[i]={directionx,directiony};
        lastId=i;
    }

    std::unordered_set<uint64_t> figures;
    for (size_t j = 0; j < population.size(); ++j) {
        sf::FloatRect box = population[j].getShape().getGlobalBounds();
        
        int left = std::floor(box.left);
        int right = std::floor(box.left + box.width  - 1);
        int top = std::floor(box.top);
        int bottom = std::floor(box.top + box.height - 1);
        
        for (int x = left; x <= right; x++) {
            figures.insert(((uint64_t)x << 32) | (uint32_t)top);
            figures.insert(((uint64_t)x << 32) | (uint32_t)bottom);
        }
        for (int y = top; y <= bottom; y++) {
            figures.insert(((uint64_t)left << 32) | (uint32_t)y);
            figures.insert(((uint64_t)right << 32) | (uint32_t)y);
        }
    }

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
                std::cout << "Mouse: " << mouse.x << ", " << mouse.y << "\n";
                long clicked = getIndividualAt(population, mouse.x, mouse.y);
                if (clicked!=-1) {
                    for (size_t i = 0; i < population.size(); ++i) {
                        if (population[i].getId() == selectedIndividual) {
                            population[i].getShape().setFillColor(sf::Color::Blue);
                        }
                    }
                    selectedIndividual = clicked;
                    std::cout << "¡Individuo seleccionado!\n";
                    for (size_t i = 0; i < population.size(); ++i) {
                        if (population[i].getId() == selectedIndividual) {
                            population[i].getShape().setFillColor(sf::Color::Red);
                        }
                    }
                }
            }
            if(buttonSpace.isClicked(event,window)){
                std::cout<<buttonSpace.getButtonText()<<std::endl;
            }
            if (buttonAddIndividual.isClicked(event,window)){
                lastId++;
                population.push_back(createIndividual(lastId));
                sf::FloatRect box = population[lastId].getShape().getGlobalBounds();
        
                int left = std::floor(box.left);
                int right = std::floor(box.left + box.width  - 1);
                int top = std::floor(box.top);
                int bottom = std::floor(box.top + box.height - 1);
                
                for (int x = left; x <= right; x++) {
                    figures.insert(((uint64_t)x << 32) | (uint32_t)top);
                    figures.insert(((uint64_t)x << 32) | (uint32_t)bottom);
                }
                for (int y = top; y <= bottom; y++) {
                    figures.insert(((uint64_t)left << 32) | (uint32_t)y);
                    figures.insert(((uint64_t)right << 32) | (uint32_t)y);
                }
            }
            if (buttonDeleteIndividual.isClicked(event, window) && selectedIndividual!=-1) {
                size_t indexToErase = 0;
                for (size_t i = 0; i < population.size(); ++i) {
                    if (population[i].getId() == selectedIndividual) {
                        indexToErase = i;
                        break;
                    }
                }

                population.erase(population.begin() + indexToErase);
                directions.erase(selectedIndividual);
                selectedIndividual=-1;
            }
            if(button3.isButtonClicked(event,window)){
                button3.setShowChoices(!button3.getShowChoices());
            }
            if(button3.isChoiceClicked(event,window)){

            }
        }

        // ---------------------------------
        //       MOVIMIENTO CONTINUO
        // ---------------------------------
        for (size_t i = 0; i < population.size(); ++i){
            std::uniform_real_distribution<float> prob(0.00f, 1.00f);
            long id = population[i].getId();
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
            if(!population[i].move(figures,dx,dy,0,60,1600,900)){
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
        window.clear(sf::Color(1, 130, 129));     

        //sf::VertexArray debugPixels(sf::Points);

        //for (uint64_t key : figures) {
        //    uint32_t y  =  key        & 0xFFFFFFFF;
        //    uint32_t x  = (key >> 32) & 0xFFFFFFFF;
        //
        //    sf::Vertex v;
        //    v.position = sf::Vector2f(x, y);
        //    v.color = sf::Color::Red;  // Pintar el pixel rojo
        //
        //    debugPixels.append(v);
        //}
        
        //window.draw(debugPixels);

        for(Individual& ind : population){
            ind.draw(window);
        }
        menuPanel.draw(window);
        populationPanel.draw(window);
        titleBar.draw(window);
        window.display();
    }

    return 0;
}

