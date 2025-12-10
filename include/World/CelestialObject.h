/*
 * The Celestial Object class represents the highest abstraction of bodies included in the simulation.
 * All vectors are in 3D format, to align with the vertex.glsl file.
 * 2D class implementations use these 3D vectors, automatically making any z element 0.0f by default, enforcing 2D behavior
 */

#ifndef OPENGLPRACTICE_CELESTIALOBJECT_H
#define OPENGLPRACTICE_CELESTIALOBJECT_H

#include <vector>
#include <cmath>
#include <random>

#include <glm/glm.hpp>

#include "Data Structs/TrailBuffer.h"

class CelestialObject {
    public:
        // Rendering vars
        unsigned int vertices_VAO;
        unsigned int trail_VBO, trail_VAO;
        unsigned int billboard_VAO;

        std::vector<float> NDC_coordinates;
        std::vector<int> NDC_indices;
        TrailBuffer trail_points;
        std::vector<float> billboard_coordinates;
        glm::vec3 color;

        // Simulation data
        glm::vec3 position;
        glm::vec3 velocity;
        float mass;
        float radius;

        CelestialObject(glm::vec3 position, glm::vec3 velocity, int segments, float mass, float radius, float pollTime, float trailDuration, glm::vec3 color) : trail_points(pollTime, trailDuration) {
            genNDCCoordinates(segments);
            genBillboardCoordinates(segments);
            this->position = position;
            this->velocity = velocity;
            this->mass = mass;
            this->radius = radius;
            this->color = color;

            trail_points.size();
        }

        void updatePosition() {
            // this->position += this->velocity;
            auto vel = this->velocity;
            vel *= 1440;
            this->position += vel;
        }

        void updateVelocity(glm::vec3 acceleration) {
            // this->velocity += acceleration;
            auto acc = acceleration;
            acc *= 1440;
            this->velocity += acc;
        }

        /**
         * This method takes the objects current position and pushes it into the trail_points vector
         */
        void addTrailPoint() {
            trail_points.addTrailPoint(position);
        }

    private:
        void genNDCCoordinates(int segments) {
            // Random number generation for object noise gen
            float noise = 0.0f;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distrib(1, 1 + noise);

            segments = static_cast<float>(segments);
            // Center of sphere object
            NDC_coordinates.push_back(0.0f);
            NDC_coordinates.push_back(0.0f);
            NDC_coordinates.push_back(0.0f);
            // Actual sphere coordinates
            for (float i = 0; i <= segments; i++) {
                float phi = 2 * M_PI * i / segments;
                for (float j = 0; j <= segments; j++) {
                    float theta = 2.0f * M_PI * j / segments;

                    float x = sin(phi) * cos(theta) * distrib(gen);
                    float y = cos(phi) * distrib(gen);
                    float z = sin(phi) * sin(theta) * distrib(gen);

                    NDC_coordinates.push_back(x);
                    NDC_coordinates.push_back(y);
                    NDC_coordinates.push_back(z);
                }
            }

            for (int i = 0; i <= segments; i++) {
                for (int j = 0; j <= segments; j++) {
                    int a = i * (segments + 1) + j;
                    int b = a + 1;
                    int c = a + (segments + 1);
                    int d = c + 1;

                    NDC_indices.push_back(a);
                    NDC_indices.push_back(c);
                    NDC_indices.push_back(b);

                    NDC_indices.push_back(b);
                    NDC_indices.push_back(c);
                    NDC_indices.push_back(d);
                }
            }
        }


        void genBillboardCoordinates(int segments) {
            segments = static_cast<float>(segments);
            billboard_coordinates.push_back(0.0f);
            billboard_coordinates.push_back(0.0f);
            for (int i = 0; i < segments; i++) {
                float sinCoord = sin(2 * M_PI * i / segments);
                float cosCoord = cos(2 * M_PI * i / segments);

                billboard_coordinates.push_back(sinCoord);
                billboard_coordinates.push_back(cosCoord);
            }
            billboard_coordinates.push_back(billboard_coordinates[2]);
            billboard_coordinates.push_back(billboard_coordinates[3]);
        }


};


#endif //OPENGLPRACTICE_CELESTIALOBJECT_H