/*
 * Re-implement delta-time for standardizing speeds across machines?
 * Delta-time can standardize times for machines that may not be able to meet the frame count.
 * With a low frame ceiling of 24 fps, machines should be able to meet these frames, don't worry about delta time for now
 */

#include <format>
#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>

#include "Graphics/Shader.h"
#include "World/Planet.h"
#include "World/Simulation.h"
#include "World/Star.h"
#include "Graphics/Renderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Renderer& renderer);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float orthoDimensions = 1000.0f;
float fov = 45.0f;
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;

const double frameDuration = 1.0f / 60.0f;
double lastFrame = 0.0f;
double lastTrailTime = 0.0f;

int segments = 25;

int main() {

    Renderer renderer(SCR_WIDTH, SCR_HEIGHT, 60.0f);

    std::unique_ptr<Star> star = std::make_unique<Star>(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), segments, 1e11f, 10);
    Simulation sim;
    // sim.addObject(std::make_unique<Star>(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), segments, 1e11f, 10));
    // sim.addObject(std::make_unique<Planet>(glm::vec3(100.0f, 0.0f, 10.0f), glm::vec3(0.0f, -0.11f, 0.0f), segments, 1e10, 5));
    // sim.addObject(std::make_unique<Planet>(glm::vec3(40.0f, 0.0f, 0.0f), glm::vec3(0.0f, -0.42f, 0.0f), segments, 1e8, 0.3));
    // sim.addObject(std::make_unique<Planet>(glm::vec3(50.0f, 0.0f, 0.0f), glm::vec3(0.0f, -0.25f, 0.0f), segments, 5e8, 1));

    sim.addObject(std::make_unique<Star>(glm::vec3(20, 0, 0), glm::vec3(0, 0, 0.3f), segments, 1e11f, 10));
    sim.addObject(std::make_unique<Star>(glm::vec3(-20, 0, 0), glm::vec3(0, 0, -0.3f), segments, 1e11f, 10));

    for (std::unique_ptr<CelestialObject>& ptr : sim.objects) {
        renderer.bufferObject(ptr);
    }

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(renderer.window)) {
        // Delta calculations
        double currentFrame = glfwGetTime();
        double timeSinceLastFrame = currentFrame - lastFrame;
        double timeSinceLastTrail = currentFrame - lastTrailTime;

        // Check if the time since last trail point was added has been passed
        if (timeSinceLastTrail > 0.1f) {
            sim.logTrailPoints();
            // Update Renderer Line Buffers HERE
            lastTrailTime = currentFrame;
        }

        // Check if the next frame is ready or if a sleep is needed
        if (timeSinceLastFrame < frameDuration) {
            double timeToNextFrame = frameDuration - timeSinceLastFrame;
            std::this_thread::sleep_for(std::chrono::duration<double>(timeToNextFrame));
        }

        // Update simulation values
        sim.simulationUpdate();

        // Read input
        processInput(renderer.window, renderer);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        renderer.shader_setup();
        renderer.drawBuffers();

        // Update last time on successful frame gen
        lastFrame = glfwGetTime();

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
    float cameraSpeed = 5.0f;
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