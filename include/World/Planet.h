#ifndef OPENGLPRACTICE_PLANET2D_H
#define OPENGLPRACTICE_PLANET2D_H

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "World/CelestialObject.h"

class Planet : virtual public CelestialObject {
    public:


    private:

};

class Planet2D : public CelestialObject2D, public Planet {
    public:
        Planet2D(int segments, glm::vec2 position, glm::vec2 velocity, float mass, float radius) :
            CelestialObject(mass, radius),
            CelestialObject2D(segments, position, velocity, mass, radius),
            Planet()
        {

        }

    private:


};


#endif //OPENGLPRACTICE_PLANET2D_H