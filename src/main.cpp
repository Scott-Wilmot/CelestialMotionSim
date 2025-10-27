#include <format>
#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Planet.h"
#include "Simulation.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const float TIMESTEP = 1.0f / 60.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float accumulatedTime = 0.0f;

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sim", nullptr, nullptr);
    if (window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glDisable(GL_CULL_FACE);

    Shader shaderProgram("..\\shaders\\vertex.glsl",
        "..\\shaders\\fragment.glsl");

    // Simulation Initialization and Setup
    Star2D star = Star2D(10, 100000000000, 100);
    Planet2D planet = Planet2D(100, 3, 2, glm::vec2(30.0f, 0.0f), glm::vec2(0.0f, -0.32f));
    Simulation2D sim = Simulation2D(star);
    sim.addPlanet(planet);

    unsigned int starVBO, planetVBO;
    unsigned int starVAO, planetVAO;
    glGenVertexArrays(1, &starVAO);
    glGenVertexArrays(1, &planetVAO);
    glGenBuffers(1, &starVBO);
    glGenBuffers(1, &planetVBO);

    // Buffer the star data
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, star.NDC_coordinates.size() * sizeof(float), star.NDC_coordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Buffer the planet data
    glBindVertexArray(planetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
    glBufferData(GL_ARRAY_BUFFER, planet.NDC_coordinates.size() * sizeof(float), planet.NDC_coordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Resets vertex array binding to none

    // Uniform locations
    int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLocation = glGetUniformLocation(shaderProgram.ID, "view");
    int projectionLocation = glGetUniformLocation(shaderProgram.ID, "projection");

    // Projection pre-step
    float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    float orthoDimensions = 50.0f;
    glm::mat4 projection = glm::ortho(-orthoDimensions * aspect, orthoDimensions * aspect, -orthoDimensions, orthoDimensions, -1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        // Delta calculations
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        accumulatedTime += deltaTime;

        while (accumulatedTime >= TIMESTEP) {
            // Update planets positions
            sim.update(deltaTime);
            accumulatedTime -= TIMESTEP;
        }

        // Read input
        processInput(window);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate shader program
        shaderProgram.use();
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Coordinate matricies
        glm::mat4 view = glm::mat4(1.0f);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // Draw star
        glBindVertexArray(starVAO);
        glm::mat4 starModel = glm::mat4(1.0f);
        starModel = glm::translate(starModel, star.world_coordinates);
        starModel = glm::scale(starModel, glm::vec3(star.radius, star.radius, 1.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(starModel));
        glDrawArrays(GL_TRIANGLE_FAN, 0, star.NDC_coordinates.size() / 2);

        // Draw planet
        glBindVertexArray(planetVAO);
        glm::mat4 planetModel = glm::mat4(1.0f);
        planetModel = glm::translate(planetModel, sim.planets[0].world_coordinates);
        planetModel = glm::scale(planetModel, glm::vec3(planet.radius, planet.radius, 1.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(planetModel));
        glDrawArrays(GL_TRIANGLE_FAN, 0, planet.NDC_coordinates.size() / 2);

        // Boilerplate
        glfwSwapBuffers(window);
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
}