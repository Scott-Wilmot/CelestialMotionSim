#include <format>
#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <vector>

#include "Graphics/Shader.h"
#include "World/Planet.h"
#include "World/Simulation.h"
#include "World/Star.h"
#include "Graphics/Renderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Renderer& renderer);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

float orthoDimensions = 1000.0f;
float fov = 45.0f;
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;

const float TIMESTEP = 1.0f / 60.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float accumulatedTime = 0.0f;

int main() {

    // Shader2D shaderProgram("..\\shaders\\vertex.glsl",
    //     "..\\shaders\\fragment.glsl", aspect, orthoDimensions);
    // std::unique_ptr<Shader> shaderProgram = std::make_unique<Shader3D>("..\\shaders\\vertex.glsl",
    //     "..\\shaders\\fragment.glsl", aspect, fov);

    Renderer renderer(SCR_WIDTH, SCR_HEIGHT, TIMESTEP, 45.0f);

    // std::unique_ptr<Star2D> star = std::make_unique<Star2D>(100, glm::vec2(0, 0), glm::vec2(0, 0), 1e11f, 10);
    std::unique_ptr<Star> star = std::make_unique<Star>(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 100, 1e11f, 10);
    Simulation sim(std::move(star));

    sim.addObject(std::make_unique<Planet>(glm::vec3(30.0f, 0.0f, 0.0f), glm::vec3(0.0f, -0.3f, 0.0f), 100, 3, 3));
    sim.addObject(std::make_unique<Planet>(glm::vec3(-30.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.32f, 0.0f), 100, 3, 0.5));
    sim.addObject(std::make_unique<Planet>(glm::vec3(50.0f, 0.0f, 0.0f), glm::vec3(0.0f, -0.25f, 0.0f), 100, 3, 1));
    // sim.addObject(std::make_unique<Planet2D>(100, glm::vec2(-30.0f, 0.0f), glm::vec2(0.0f, 0.32f), 3, 2));
    // sim.addObject(std::make_unique<Planet2D>(100, glm::vec2(50.0f, 0.0f), glm::vec2(0.0f, -0.25f), 3, 2));

    renderer.bufferObject(sim.star);
    for (std::unique_ptr<CelestialObject>& ptr : sim.planets) {
        renderer.bufferObject(ptr);
    }

    // glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(renderer.window)) {
        // Delta calculations
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        accumulatedTime += deltaTime;

        while (accumulatedTime >= TIMESTEP) {
            // Update planets positions
            sim.update();
            accumulatedTime -= TIMESTEP;
        }

        // Read input
        processInput(renderer.window, renderer);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        renderer.shader_setup();

        renderer.drawObject(sim.star);
        for (std::unique_ptr<CelestialObject>& ptr : sim.planets) {
            renderer.drawObject(ptr);
        }

        // Boilerplate
        glfwSwapBuffers(renderer.window);
        glfwPollEvents();

    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Renderer& renderer) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    //     shader.updateProjection(3.0f);
    // }
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //     shader.updateProjection(-3.0f);
    // }
    //
    float cameraSpeed = 50.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        renderer.update_camera_position(FORWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        renderer.update_camera_position(BACKWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        renderer.update_camera_position(LEFT, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        renderer.update_camera_position(RIGHT, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        renderer.update_camera_position(UP, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        renderer.update_camera_position(DOWN, cameraSpeed);
    }
}