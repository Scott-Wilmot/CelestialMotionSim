#ifndef OPENGLPRACTICE_RENDERER_H
#define OPENGLPRACTICE_RENDERER_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <any>
#include <unordered_map>

#include "Graphics/Camera.h"
#include "Graphics/Shader.h"
#include "World/CelestialObject.h"

class Renderer {
    public:
        // Shader variables
        std::unique_ptr<Shader> shader;
        std::unique_ptr<Camera> camera;

        GLFWwindow* window;
        int SCR_WIDTH, SCR_HEIGHT; // Is there any reason to keep this? Aspect ratio shenanagains
        float ASPECT_RATIO;
        float zoomFactor = 1.0f;

        std::unordered_map<const CelestialObject*, unsigned int> vao_map;

        // Object to VBO & VAO map for line buffers, VBO first, VAO second
        std::unordered_map<const CelestialObject*, std::pair<unsigned int, unsigned int>> trail_map;

        Renderer(float FOV) {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // Getting the monitor
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            SCR_WIDTH = mode->width;
            SCR_HEIGHT = mode->height;
            ASPECT_RATIO = (float)SCR_WIDTH / (float)SCR_HEIGHT;

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
            glfwSetScrollCallback(window, scroll_callback);

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
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Comment this line out for sphere view
            glfwSetWindowPos(window, 0.0f, 0.0f);
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
                object->NDC_indices.size() * sizeof(int),
                object->NDC_indices.data(),
                GL_STATIC_DRAW
                );

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Store ptr and VAO in map
            vao_map[object.get()] = VAO;

            // Trail buffer initialization
            unsigned int trailVBO, trailVAO;
            glGenVertexArrays(1, &trailVAO);
            glGenBuffers(1, &trailVBO);

            glBindVertexArray(trailVAO);
            glBindBuffer(GL_ARRAY_BUFFER, trailVBO);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            auto trail_buffers = std::pair<unsigned int, unsigned int>(trailVBO, trailVAO);
            trail_map[object.get()] = trail_buffers;

            // Cleanup
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }


        /**
         * Draws a single object passed in as a parameter.
         * Requires an external for loop to go through all objects in the simulation.
         * Future Deprecation: All object and VAO data is stored in the local hashmap, data is not changed, why not just cycle through keys and render all with a single method call?
         * @param object
         * @deprecated This method has been replaced with the drawBuffers() method
         * @see Renderer::drawBuffers()
         */
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
            glDrawElements(GL_TRIANGLES, object->NDC_indices.size(), GL_UNSIGNED_INT, 0);
            // glDrawArrays(GL_POINTS, 0, object->NDC_coordinates.size());
        }

        /**
         * Intended replacement for drawObject()
         * Does not change any buffer data, rather accesses and draws all vaos with object data considered.
         * Grabs all keys from hashmap to get access to VAOs and relevant object data
         */
        void drawBuffers() {
            for (const auto& object_ptr : vao_map) {    //OBJECT_PTR IS NOT A CORRECT NAME
                // Object rendering
                unsigned int VAO = object_ptr.second;
                float radius = object_ptr.first->radius;

                glBindVertexArray(VAO);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, object_ptr.first->position / zoomFactor);
                model = glm::scale(model, glm::vec3(radius, radius, radius) / zoomFactor);
                shader->set_model(model);

                glDrawElements(GL_TRIANGLES, object_ptr.first->NDC_indices.size(), GL_UNSIGNED_INT, 0);

                //////////////////
                // Trail rendering
                //////////////////

                // Set model uniform to identity matrix to stop line from moving with objects
                model = glm::mat4(1.0f);
                shader->set_model(model);

                // Update the trail buffer
                updateTrailBuffer(object_ptr.first, zoomFactor);

                // Get and draw the trail VAO
                unsigned int trailVAO = trail_map[object_ptr.first].second; // Note: .second grabs the VAO from the pair value in the map
                glBindVertexArray(trailVAO);
                glDrawArrays(GL_LINE_STRIP, 0, object_ptr.first->trail_points.size());
            }
        }

        /**
         * Method that updates the buffers present in line_map to reflect the current line points
         */
        void updateTrailBuffer(const CelestialObject* obj_ptr, float zoomFactor) {
            unsigned int trailVBO = trail_map[obj_ptr].first;
            std::vector<glm::vec3> zoomed_points;

            std::vector<glm::vec3> trail_pts = obj_ptr->trail_points.trail_points;
            for (const auto& position_vec : trail_pts) {
                zoomed_points.push_back(position_vec / zoomFactor);
            }

            // Pushes data to buffer, VAO automatically picks it up
            glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
            glBufferData(GL_ARRAY_BUFFER,
                obj_ptr->trail_points.size() * sizeof(glm::vec3),
                zoomed_points.data(),
                GL_DYNAMIC_DRAW);
        }

        /**
         * This function activates the Shader program from the Shader object
         * This function sets the uniforms for view and projection in the Shader class
         * Does this function need to update the matricies before using them to update the uniforms? I think so.
         */
        void shader_setup() {
            shader->use();

            camera->update_view_matrix();
            shader->set_view(camera->view);

            shader->set_projection(camera->projection);
        }

        void update_camera_position(CameraMovement direction, float cameraSpeed) const {
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

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
            Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
            if (renderer) {
                renderer->onScroll(xoffset, yoffset);
            }
        }

        void onScroll(double xOffset, double yOffset) {
            // zoomFactor *= 0.99;
            if (yOffset > 0) {  // Indicates up scroll
                zoomFactor *= 0.9f; // Shrink distances, "zoom in"
            }
            if (yOffset < 0) {  // Indicates down scroll
                zoomFactor *= 1.1f; // Widen distances, "zoom out"1
            }
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