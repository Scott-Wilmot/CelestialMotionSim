//
// Created by scott on 10/26/2025.
//

#ifndef OPENGLPRACTICE_STAR_H
#define OPENGLPRACTICE_STAR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Star {
    public:
        float radius;
        float mass;
        glm::vec3 world_coordinates;

        Star(float radius, float mass) {
            this->radius = radius;
            this->mass = mass;
        }

    private:
};

class Star2D {
    public:
        float radius;
        float mass;

        glm::vec2 position;
        std::vector<float> NDC_coordinates;
        glm::vec3 world_coordinates;

        Star2D(float radius, float mass, int segments) {
            this->radius = radius;
            this->mass = mass;

            this->position = glm::vec2(0, 0);
            world_coordinates = glm::vec3(position, 0);
            genStar(segments);
        }

    private:
        void genStar(int segments) {
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

class Star3D : public Star {

};

#endif //OPENGLPRACTICE_STAR_H