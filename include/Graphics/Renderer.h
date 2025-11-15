#ifndef OPENGLPRACTICE_RENDERER_H
#define OPENGLPRACTICE_RENDERER_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <any>
#include <unordered_map>

class Renderer {
    public:
        GLFWwindow* window;
        int SCR_WIDTH, SCR_HEIGHT; // Is there any reason to keep this? Aspect ratio shenanagains
        float ASPECT_RATIO;
        float FPS;

        std::unordered_map<const CelestialObject*, unsigned int> vao_map;

        Renderer(int SCR_WIDTH, int SCR_HEIGHT, float FPS) {
            this->SCR_WIDTH = SCR_WIDTH;
            this->SCR_HEIGHT = SCR_HEIGHT;
            this->FPS = FPS;

            ASPECT_RATIO = (float)SCR_WIDTH / (float)SCR_HEIGHT;

            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine", NULL, NULL);
            if (window == NULL) {
                std::cout << "Failed to create window" << std::endl;
                glfwTerminate();
                throw std::exception();
            }
            glfwMakeContextCurrent(window);
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initializ GLAD" << std::endl;
                throw std::exception();
            }

            glDisable(GL_CULL_FACE);
        }

        void bufferObject(std::unique_ptr<CelestialObject>& object) {
            // Init buffers
            unsigned int VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            // Buffer object data
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                object->NDC_coordinates.size() * sizeof(float),
                object->NDC_coordinates.data(),
                GL_STATIC_DRAW
                );
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Store ptr and VAO in map
            vao_map[object.get()] = VAO;

            // Cleanup
            glBindVertexArray(0);
        }

        void drawObject(std::unique_ptr<CelestialObject>& object, int modelLocation) {
            // Get the parameter pointer objects associated VAO
            unsigned int VAO = vao_map[object.get()];
            float r = object->radius;

            // Apply objects model matrix to Shader
            glBindVertexArray(VAO);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, object->position);
            model = glm::scale(model, glm::vec3(r, r, r));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            // Draw the object
            glDrawArrays(GL_TRIANGLE_FAN, 0, object->NDC_coordinates.size() / 3);
        }

    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        }
};

#endif //OPENGLPRACTICE_RENDERER_H