#ifndef OPENGLPRACTICE_RENDERER_H
#define OPENGLPRACTICE_RENDERER_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <any>
#include <unordered_map>

#include "Graphics/Camera.h"
#include "Graphics/Shader.h"

class Renderer {
    public:
        // Shader variables
        std::unique_ptr<Shader> shader;
        std::unique_ptr<Camera> camera;

        GLFWwindow* window;
        int SCR_WIDTH, SCR_HEIGHT; // Is there any reason to keep this? Aspect ratio shenanagains
        float ASPECT_RATIO;
        float FPS;

        std::unordered_map<const CelestialObject*, unsigned int> vao_map;

        Renderer(int SCR_WIDTH, int SCR_HEIGHT, float FPS, float FOV) {
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
            glfwSetWindowUserPointer(window, this);
            glfwSetCursorPosCallback(window, mouse_callback);

            // Set mouse input to GLFW window
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initializ GLAD" << std::endl;
                throw std::exception();
            }

            shader = std::make_unique<Shader>("..\\shaders\\vertex.glsl",
                "..\\shaders\\fragment.glsl", ASPECT_RATIO);
            camera = std::make_unique<Camera>(FOV, ASPECT_RATIO);

            glDisable(GL_CULL_FACE);
        }

        void bufferObject(std::unique_ptr<CelestialObject>& object) {
            // Init buffers
            unsigned int VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            // Buffer object data
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                object->NDC_coordinates.size() * sizeof(float),
                object->NDC_coordinates.data(),
                GL_STATIC_DRAW
                );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                object->indices.size() * sizeof(int),
                object->indices.data(),
                GL_STATIC_DRAW
                );

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Store ptr and VAO in map
            vao_map[object.get()] = VAO;

            // Cleanup
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void drawObject(std::unique_ptr<CelestialObject>& object) {
            // Get the parameter pointer objects associated VAO
            unsigned int VAO = vao_map[object.get()];
            float r = object->radius;

            // Apply objects model matrix to Shader
            glBindVertexArray(VAO);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, object->position);
            model = glm::scale(model, glm::vec3(r, r, r));
            shader->set_model(model);

            // Draw the object
            // glDrawArrays(GL_TRIANGLE_FAN, 0, object->NDC_coordinates.size() / 3);
            // glPointSize(5.0f);
            glDrawElements(GL_TRIANGLES, object->indices.size(), GL_UNSIGNED_INT, 0);
            // glDrawArrays(GL_POINTS, 0, object->NDC_coordinates.size());
        }

        /**
         * This function activates the Shader program from the Shader object
         * This function sets the uniforms for view and projection in the Shader class
         * Does this function need to update the matricies before using them to update the uniforms? I think so.
         */
        void shader_setup() {
            shader->use();

            shader->set_view(camera->view);
            shader->set_projection(camera->projection);

            camera->update_view_matrix();
        }

        void update_camera_position(CameraMovement direction, float cameraSpeed) {
            camera->update_camera_position(direction, cameraSpeed);
        }


    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        }

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
            Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
            renderer->callCameraMouse(xpos, ypos);
        }

        void callCameraMouse(double xpos, double ypos) {
            camera->mouse_callback(xpos, ypos);
        }

        void checkCompileErrors(unsigned int shader, std::string type) {
            int success;
            char infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type : " << type << "\n" << infoLog << "\n -- -------------------------------------------------------------------------- --" << std::endl;
                }
            }
            else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_PROGRAM_LINKING_ERROR of type : " << type << "\n" << infoLog << "\n -- -------------------------------------------------------------------------- --" << std::endl;
                }
            }
        }

};

#endif //OPENGLPRACTICE_RENDERER_H