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
        std::vector<std::unique_ptr<CelestialObject>> objects;

        float G = 6.6743e-11f; // Newtons gravitiational constant

        Simulation() {

        }

        // Trade ownership of the object pointer to the array
        void addObject(std::unique_ptr<CelestialObject> obj) {
            objects.push_back(std::move(obj));
        }

        // /**
        //  * Implementation of object update, covering only planet objects.
        //  * Star objects remain static with this implementation.
        //  */
        // void update() {
        //     float G = 6.6743e-11f; // Newtons gravitiational constant
        //     for (std::unique_ptr<CelestialObject>& planet : planets) {
        //         glm::vec3 diff = star->position - planet->position;
        //         float r = glm::length(diff);
        //
        //         if (r == 0.0f) continue;
        //         glm::vec3 dir = diff / r;
        //
        //         float F = (G * star->mass) / (r * r);
        //         glm::vec3 force = dir * F;
        //
        //         glm::vec3 accel = force / planet->mass;
        //
        //         if (CelestialObject* c = dynamic_cast<CelestialObject*>(planet.get())) {
        //             c->updateVelocity(accel);
        //             c->updatePosition();
        //         }
        //     }
        // }

        /**
         * Implementation of simulation update that calculates the gravitational force of each object on every other object
         * This is a brute approach to the calculation, see FMM for more efficient methods for calculating a large number of objects
         * This method take an iterative approach, since object positions cannot be updated until every objects velocity is solved
         * TODO: Add a limit so if the planets center is closer to the object it is calculating against it doesn't calculate as to avoid slingshoting
         */
        void simulationUpdate() {
            // Loop for updating the velocities of each object
            for (auto& focusObj : objects) {
                for (auto& comparisonObj : objects) {
                    // Check if the objects are the same, if yes skip loop
                    if (focusObj == comparisonObj) {
                        std::cout << "SAME OBJECT" << std::endl;
                        continue;
                    }

                    glm::vec3 position_difference = comparisonObj->position - focusObj->position;
                    float radius = glm::length(position_difference);

                    if (radius == 0.0f) continue;
                    glm::vec3 direction_unit_vector = position_difference / radius;
                    glm::vec3 acceleration = (G * comparisonObj->mass * direction_unit_vector) / (radius * radius);

                    focusObj->updateVelocity(acceleration);
                }
            }

            // Loop for updating the positions of each object after all velocities have been calculated
            for (auto& obj : objects) {
                obj->updatePosition();
            }
        }

};

#endif //OPENGLPRACTICE_SIMULATION_H