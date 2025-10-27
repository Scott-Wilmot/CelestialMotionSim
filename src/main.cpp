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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    Star2D star = Star2D(5, 10, 100);
    Planet2D planet = Planet2D(100, 1, 2, 2);
    Simulation2D sim = Simulation2D(star);
    sim.addPlanet(planet);

    // unsigned int VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    //
    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER,
    //     planet.NDC_coordinates.size() * sizeof(float),
    //         planet.NDC_coordinates.data(),
    //         GL_STATIC_DRAW);
    //
    // glVertexAttribPointer(0,
    //     2,
    //     GL_FLOAT,
    //     GL_FALSE,
    //     2 * sizeof(float),
    //     (void*)0
    //     );
    // glEnableVertexAttribArray(0);

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
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window)) {

        // Read input
        processInput(window);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Coordinate matricies
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // Activate shader program
        shaderProgram.use();

        // Draw star
        glBindVertexArray(starVAO);
        glm::mat4 starModel = glm::mat4(1.0f);
        starModel = glm::translate(starModel, planet.world_coordinates);
        starModel = glm::scale(starModel, glm::vec3(star.radius, star.radius, 1.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(starModel));
        glDrawArrays(GL_TRIANGLE_FAN, 0, star.NDC_coordinates.size() / 2);

        // Draw planet
        glBindVertexArray(planetVAO);
        glm::mat4 planetModel = glm::mat4(1.0f);
        planetModel = glm::translate(planetModel, planet.world_coordinates);
        planetModel = glm::scale(planetModel, glm::vec3(planet.radius, planet.radius, 1.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(planetModel));
        glDrawArrays(GL_TRIANGLE_FAN, 0, planet.NDC_coordinates.size() / 2);

        // Boilerplate
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}