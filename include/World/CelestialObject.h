/*
 * The Celestial Object class represents the highest abstraction of bodies included in the simulation.
 * All vectors are in 3D format, to align with the vertex.glsl file.
 * 2D class implementations use these 3D vectors, automatically making any z element 0.0f by default, enforcing 2D behavior
 */

#ifndef OPENGLPRACTICE_CELESTIALOBJECT_H
#define OPENGLPRACTICE_CELESTIALOBJECT_H

#include <vector>
#include <cmath>

#include <glm/glm.hpp>

class CelestialObject {
    public:
        std::vector<float> NDC_coordinates;
        glm::vec3 position;
        glm::vec3 velocity;

        float mass;
        float radius;

        virtual ~CelestialObject() = default;

        CelestialObject(float mass, float radius) {
            this->mass = mass;
            this->radius = radius;
        }

    private:
        virtual void genNDCCoordinates(int segments) = 0; // This makes CelestialObject abstract, cannon be directly instatntaied
};

class CelestialObject2D : virtual public CelestialObject {
    public:
        CelestialObject2D(int segments, glm::vec2 position, glm::vec2 velocity, float mass, float radius) :
            CelestialObject(mass, radius) {
                CelestialObject2D::genNDCCoordinates(segments);
                this->position = glm::vec3(position, 0);
                this->velocity = glm::vec3(velocity, 0);
        }

        void updatePosition() {
            this->position += this->velocity;
        }

        void updateVelocity(glm::vec3 acceleration) {
            this->velocity += acceleration;
        }

    private:
        void genNDCCoordinates(int segments) override {
            segments = static_cast<float>(segments);
            // Center of the circle points
            NDC_coordinates.push_back(0.0f);    // X
            NDC_coordinates.push_back(0.0f);    // Y
            NDC_coordinates.push_back(0.0f);    // Z
            // Generate ring points
            for (float i = 0; i < segments; i++) {
                float sin_coord = sin((2 * M_PI) * (i / segments));
                float cos_coord = cos((2 * M_PI) * (i / segments));
                NDC_coordinates.push_back(sin_coord);   // X
                NDC_coordinates.push_back(cos_coord);   // Y
                NDC_coordinates.push_back(0.0f);      // Z
            }
            // Close ring
            NDC_coordinates.push_back(NDC_coordinates[3]);
            NDC_coordinates.push_back(NDC_coordinates[4]);
            NDC_coordinates.push_back(NDC_coordinates[5]);
        }
};

class CelestialObject3D : CelestialObject {
    public:

    private:
    // Parametric equations
};

#endif //OPENGLPRACTICE_CELESTIALOBJECT_H