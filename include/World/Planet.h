#ifndef OPENGLPRACTICE_PLANET2D_H
#define OPENGLPRACTICE_PLANET2D_H

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "World/CelestialObject.h"

class Planet : public CelestialObject {
    public:
        Planet(glm::vec3 position, glm::vec3 velocity, int segments, float mass, float radius) :
            CelestialObject(position, velocity, segments, mass, radius)
        {

        }

    private:

};

#endif //OPENGLPRACTICE_PLANET2D_H