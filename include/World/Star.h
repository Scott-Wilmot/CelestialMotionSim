//
// Star class should handle lighting aspects once we get there
//

#ifndef OPENGLPRACTICE_STAR_H
#define OPENGLPRACTICE_STAR_H

#include <glm/glm.hpp>

class Star : public CelestialObject {
    public:
        Star(glm::vec3 position, glm::vec3 velocity, int segments, float mass, float radius) :
            CelestialObject(position, velocity, segments, mass, radius)
        {

        }

    private:

};

#endif //OPENGLPRACTICE_STAR_H