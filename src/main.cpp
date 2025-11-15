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
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

float orthoDimensions = 1000.0f;
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;

const float TIMESTEP = 1.0f / 60.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float accumulatedTime = 0.0f;

int main() {

    Renderer renderer(SCR_WIDTH, SCR_HEIGHT, TIMESTEP);

    Shader2D shaderProgram("..\\shaders\\vertex.glsl",
        "..\\shaders\\fragment.glsl", aspect, ortho.get());

    std::unique_ptr<Star2D> star = std::make_unique<Star2D>(100, glm::vec2(0, 0), glm::vec2(0, 0), 1e11f, 10);
    Simulation sim(std::move(star));

    sim.addObject(std::make_unique<Planet2D>(100, glm::vec2(30.0f, 0.0f), glm::vec2(0.0f, -0.3f), 3, 2));
    sim.addObject(std::make_unique<Planet2D>(100, glm::vec2(-30.0f, 0.0f), glm::vec2(0.0f, 0.32f), 3, 2));
    sim.addObject(std::make_unique<Planet2D>(100, glm::vec2(50.0f, 0.0f), glm::vec2(0.0f, -0.25f), 3, 2));

    renderer.bufferObject(sim.star);
    for (std::unique_ptr<CelestialObject>& ptr : sim.planets) {
        renderer.bufferObject(ptr);
    }

    // Uniform locations
    int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLocation = glGetUniformLocation(shaderProgram.ID, "view");
    int projectionLocation = glGetUniformLocation(shaderProgram.ID, "projection");

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
        processInput(renderer.window);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate shader program
        shaderProgram.use();
        glm::mat4 projection = glm::ortho(-orthoDimensions * aspect.get(), orthoDimensions * aspect, -orthoDimensions, orthoDimensions, -1.0f, 1.0f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Coordinate matricies
        glm::mat4 view = glm::mat4(1.0f);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));


        renderer.drawObject(sim.star, modelLocation);
        for (std::unique_ptr<CelestialObject>& ptr : sim.planets) {
            renderer.drawObject(ptr, modelLocation);
        }

        // Boilerplate
        glfwSwapBuffers(renderer.window);
        glfwPollEvents();

    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        orthoDimensions += 0.5;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        orthoDimensions -= 0.5;
    }
}