#ifndef OPENGLPRACTICE_PLANET2D_H
#define OPENGLPRACTICE_PLANET2D_H

#include <glm/glm.hpp>

#include "World/CelestialObject.h"

class Planet : public CelestialObject {
    public:
        Planet(glm::vec3 position, glm::vec3 velocity, int segments, float mass, float radius, float pollTime, float trailDuration) :
            CelestialObject(position, velocity, segments, mass, radius, pollTime, trailDuration)
        {

        }

    private:

};

#endif //OPENGLPRACTICE_PLANET2D_H