#ifndef OPENGLPRACTICE_SIMULATION_H
#define OPENGLPRACTICE_SIMULATION_H

#include <vector>
#include <memory>
#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

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

        /**
         * Process a JSON file with object data into simulation objects automatically
         */
        void jsonToObjects() {
            std::ifstream jsonFile("..\\planetData\\objects.json");
            json data = json::parse(jsonFile);
            for (auto object : data["objects"]) {
                double mass = object["mass"];
                double radius = object["radius"]; radius *= 1000;
                glm::vec3 position = glm::vec3(object["X"], object["Y"], object["Z"]); position *= 1000;
                glm::vec3 velocity = glm::vec3(object["VX"], object["VY"], object["VZ"]); velocity *= 1000;

                // std::cout << object["name"] << std::endl
                //     << mass << std::endl
                //     << radius << std::endl
                //     << position.x << ", " << position.y << ", " << position.z << std::endl
                //     << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl << std::endl;

                addObject(std::make_unique<CelestialObject>(position, velocity, 30, mass, radius, 0.1f, 5.0f));
            }
        }

        // Trade ownership of the object pointer to the array
        void addObject(std::unique_ptr<CelestialObject> obj) {
            objects.push_back(std::move(obj));
        }

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
                        continue;
                    }

                    glm::vec3 position_difference = comparisonObj->position - focusObj->position;
                    float radius = glm::length(position_difference);

                    if (radius <= comparisonObj->radius + focusObj->radius) continue;
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

        /**
         * Method for adding a trail point to each object in the simulation
         * Logged point is the objects current position
         */
        void logTrailPoints() {
            for (auto& object : objects) {
                object->addTrailPoint();
            }
        }

};

#endif //OPENGLPRACTICE_SIMULATION_H