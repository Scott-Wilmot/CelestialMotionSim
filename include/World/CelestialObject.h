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

class CelestialObject {
    public:
        std::vector<float> NDC_coordinates;
        std::vector<int> indices;

        glm::vec3 position;
        glm::vec3 velocity;

        float mass;
        float radius;

        CelestialObject(glm::vec3 position, glm::vec3 velocity, int segments, float mass, float radius) {
            genNDCCoordinates(segments);
            this->position = position;
            this->velocity = velocity;
            this->mass = mass;
            this->radius = radius;
        }

        void updatePosition() {
            this->position += this->velocity;
        }

        void updateVelocity(glm::vec3 acceleration) {
            this->velocity += acceleration;
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

                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(b);

                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }
    }
};


#endif //OPENGLPRACTICE_CELESTIALOBJECT_H