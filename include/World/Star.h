//
// Created by scott on 10/26/2025.
//

#ifndef OPENGLPRACTICE_STAR_H
#define OPENGLPRACTICE_STAR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Star : virtual public CelestialObject {
    public:


    private:

};

class Star2D : public CelestialObject2D, public Star {
    public:
        Star2D(int segments, glm::vec2 position, glm::vec2 velocity, float mass, float radius) :
            CelestialObject(mass, radius),
            CelestialObject2D(segments, position, velocity, mass, radius),
            Star()
        {

        }

    private:

};

class Star3D : public Star {

};

#endif //OPENGLPRACTICE_STAR_H