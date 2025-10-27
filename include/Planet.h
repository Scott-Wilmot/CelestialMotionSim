#ifndef OPENGLPRACTICE_PLANET2D_H
#define OPENGLPRACTICE_PLANET2D_H

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Planet {
    public:
        virtual ~Planet() = default;

        std::vector<float> NDC_coordinates;
        glm::vec3 world_coordinates;
        float radius;

        Planet(float radius) {
            this->radius = radius;
        }

    private:
        virtual void genPlanet(int segements) = 0;
};

class Planet2D {
    public:
        std::vector<float> NDC_coordinates;
        glm::vec3 world_coordinates;

        float radius;
        float mass;
        glm::vec2 position;
        glm::vec2 velocity;

        Planet2D(int segments, float radius, float mass, glm::vec2 position, glm::vec2 velocity) {
            this->radius = radius;
            this->position = position;
            this->velocity = velocity;
            this->mass = mass;

            genPlanet(segments);
            world_coordinates = glm::vec3(position, 0.0);
        }

        void updatePosition() {
            position += velocity;
            world_coordinates = glm::vec3(position, 0.0);
        }

    private:
        void genPlanet(int segments) {
            // Push in center of circle
            NDC_coordinates.push_back(0.0f);
            NDC_coordinates.push_back(0.0f);
            // Generate ring points
            for (float i = 0; i < segments; i++) {
                float sin_coord = sin((2 * M_PI) * (i / (float)segments));
                float cos_coord = cos((2 * M_PI) * (i / (float)segments));
                NDC_coordinates.push_back(sin_coord);
                NDC_coordinates.push_back(cos_coord);
            }
            // Add the first point again to close the fan
            NDC_coordinates.push_back(NDC_coordinates[2]);
            NDC_coordinates.push_back(NDC_coordinates[3]);
        }

};


#endif //OPENGLPRACTICE_PLANET2D_H