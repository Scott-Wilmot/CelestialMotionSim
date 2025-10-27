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
#include <cmath>

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

        /*
         * Calculates the amount of gravitaional acceleration a planet has from the star, changes the velocity, then updates position
         */
        void update() {
            for (Planet2D& planet : planets) {
                glm::vec2 r = star.position - planet.position;
                float dist = glm::length(r);

                if (dist == 0.0f) continue;
                glm::vec2 dir = r / dist;

                float G = 6.6743e-11f;
                float f = (G * star.mass) / (dist * dist);
                glm::vec2 force = dir * f;

                glm::vec2 accel = force / planet.mass;
                planet.velocity += accel;
                std::cout << accel.x << " " << accel.y << std::endl;
                std::cout << planet.velocity.x << " " << planet.velocity.y << std::endl;

                planet.updatePosition();
            }
        }
};

class Simulation3D: public Simulation {

};

#endif //OPENGLPRACTICE_SIMULATION_H