#ifndef COLORS_H
#define COLORS_H

#include <glm\glm.hpp>
#include <unordered_map>
#include <string>

namespace Colors {
    inline const std::unordered_map<std::string, glm::vec3> colors = {
        {"RED", glm::vec3(1.0f, 0.0f, 0.0f) },
        {"GREEN", glm::vec3(0.0f, 1.0f, 0.0f) },
        {"BLUE", glm::vec3(0.0f, 0.0f, 1.0f) },
        {"YELLOW", glm::vec3(1.0f, 1.0f, 0.0f) },
        {"WHITE", glm::vec3(1.0f, 1.0f, 1.0f) },
        {"GREY", glm::vec3(0.5f, 0.5f, 0.5f) },
        {"ORANGE", glm::vec3(0.5f, 0.5f, 1.0f) },
        {"TAN", glm::vec3(0.82f, 0.7f, 0.54f) },
    };
}

#endif //COLORS_H
