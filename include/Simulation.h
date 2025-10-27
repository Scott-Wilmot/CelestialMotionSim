/*
 * Class intended to hold the information for the simulation space
 * This class should hold the following properties:
 *  - Central star
 *  - list of planets
 */

#ifndef OPENGLPRACTICE_SIMULATION_H
#define OPENGLPRACTICE_SIMULATION_H

#include <vector>
#include <memory>

#include "Planet.h"
#include "Star.h"

class Simulation {
    public:
        std::unique_ptr<Star> main_star;
        std::vector<std::unique_ptr<Planet>> planets;

        /*
         * Constructor for both 2D and 3D which takes any star type and assigns it to main_star
         */
        Simulation(std::unique_ptr<Star> star) {
            main_star = std::move(star);
        }
};

/*
 * Implement polymorphism at a later time
 */
class Simulation2D {
    public:
        Star2D star;
        std::vector<Planet2D> planets;

        Simulation2D(Star2D star) : star(star) { }

        void addPlanet(Planet2D planet) {
            planets.push_back(planet);
        }
};

class Simulation3D: public Simulation {

};

#endif //OPENGLPRACTICE_SIMULATION_H