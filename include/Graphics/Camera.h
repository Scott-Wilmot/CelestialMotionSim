#ifndef OPENGLPRACTICE_CAMERA_H
#define OPENGLPRACTICE_CAMERA_H

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/CameraMovement.h"

class Camera {
  public:
    // Something constants
    float ASPECT_RATIO;
    float FOV;

    // ___ matricies
    glm::mat4 view;
    glm::mat4 projection;

    // Camera matricies
    glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 100.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Mouse input tracking
    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0f / 2.0;

    Camera(float FOV, float ASPECT_RATIO) {
        this->FOV = FOV;
        this->ASPECT_RATIO = ASPECT_RATIO;

        view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
            );
        projection = glm::perspective(
            glm::radians(FOV),
            ASPECT_RATIO,
            0.1f,
            10000.0f
        );
    }

    void update_view_matrix() {
        view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
            );
    }

    void update_camera_position(CameraMovement direction, float cameraSpeed) {
        if (direction == FORWARD)
            cameraPos += cameraSpeed * cameraFront;
        if (direction == BACKWARD)
            cameraPos -= cameraSpeed * cameraFront;
        if (direction == LEFT)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (direction == RIGHT)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (direction == DOWN)
            cameraPos -= cameraSpeed * cameraUp;
        if (direction == UP)
            cameraPos += cameraSpeed * cameraUp;
    }

    void mouse_callback(double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }

};

#endif //OPENGLPRACTICE_CAMERA_H