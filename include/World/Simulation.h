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

/**
 * Simulation implementation that supports a single star and many planets.
 * The star is in a static position.
 * Each planet only calculates it's rgavitational force from the star.
 */
class Simulation {
    public:
        std::vector<std::unique_ptr<CelestialObject>> planets;
        std::unique_ptr<CelestialObject> star;

        Simulation(std::unique_ptr<CelestialObject> star) {
            this->star = std::move(star);
        }

        // Trade ownership of the object pointer to the array
        void addObject(std::unique_ptr<CelestialObject> obj) {
            planets.push_back(std::move(obj));
        }

        /**
         * Implementation of object update, covering only planet objects.
         * Star objects remain static with this implementation.
         */
        void update() {
            float G = 6.6743e-11f; // Newtons gravitiational constant
            for (std::unique_ptr<CelestialObject>& planet : planets) {
                glm::vec3 diff = star->position - planet->position;
                float r = glm::length(diff);

                if (r == 0.0f) continue;
                glm::vec3 dir = diff / r;

                float F = (G * star->mass) / (r * r);
                glm::vec3 force = dir * F;

                glm::vec3 accel = force / planet->mass;

                if (CelestialObject* c = dynamic_cast<CelestialObject*>(planet.get())) {
                    c->updateVelocity(accel);
                    c->updatePosition();
                }
            }
        }

};

#endif //OPENGLPRACTICE_SIMULATION_H