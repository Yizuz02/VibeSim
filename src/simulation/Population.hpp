#pragma once

#include <unordered_map>
#include <memory>
#include <utility>
#include "Individual.hpp"
#include "Space.hpp"
#include "../styles/Theme.hpp"

class Population {
private:
    std::unordered_map<long, std::unique_ptr<Individual>> individuals;
    long nextId;

    float radius;          // 🔹 Radio base de la población
    Space* space;
    Theme* theme;

public:
    // Constructor
    Population(Space& space, Theme& theme, float radius);

    // Crear individuo 
    Individual& createIndividual(std::pair<int,int> pos);
    Individual& createIndividual();

    // Agregar individuo externo
    void addIndividual(std::unique_ptr<Individual> individual);

    // Regresar individuo
    long getIndividualAt(float x, float y) const;
    Individual* getIndividual(long id);
    const Individual* getIndividual(long id) const;

    std::unordered_map<long, std::unique_ptr<Individual>>& getIndividuals();
    const std::unordered_map<long, std::unique_ptr<Individual>>& getIndividuals() const;

    // Eliminar uno o todos
    bool removeIndividual(long id);
    void clear();

    // Render
    void draw();

    // Getters / setters
    float getRadius() const;
    void setRadius(float newRadius);

    size_t size() const;
    bool empty() const;
};
