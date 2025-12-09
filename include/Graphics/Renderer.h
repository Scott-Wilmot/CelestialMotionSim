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
                "..\\shaders\\fragment.glsl",
                "..\\shaders\\billboard.glsl",
                ASPECT_RATIO
                );
            camera = std::make_unique<Camera>(FOV, SCR_WIDTH, SCR_HEIGHT);

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
            object->vertices_VAO = VAO;

            // Trail buffer initialization
            unsigned int trailVBO, trailVAO;
            glGenVertexArrays(1, &trailVAO);
            glGenBuffers(1, &trailVBO);

            glBindVertexArray(trailVAO);
            glBindBuffer(GL_ARRAY_BUFFER, trailVBO);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            object->trail_VBO = trailVBO;
            object->trail_VAO = trailVAO;

            ////////////////////
            // VAO for billboard object
            ////////////////////
            unsigned int billboardVBO, billboardVAO;
            glGenVertexArrays(1, &billboardVAO);
            glGenBuffers(1, &billboardVBO);

            glBindVertexArray(billboardVAO);
            glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
            glBufferData(GL_ARRAY_BUFFER, object->billboard_coordinates.size() * sizeof(float), object->billboard_coordinates.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            object->billboard_VAO = billboardVAO;

            // Cleanup
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }


        /**
         * Intended replacement for drawObject()
         * Does not change any buffer data, rather accesses and draws all vaos with object data considered.
         * Grabs all keys from hashmap to get access to VAOs and relevant object data
         */
        void drawBuffers(Simulation& sim) {
            for (const auto& object : sim.objects) {    //OBJECT_PTR IS NOT A CORRECT NAME
                // Object rendering
                float radius = object->radius;

                // shader->set_projection(camera->perspective_projection);

                // Draw sphere
                glBindVertexArray(object->vertices_VAO);
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 rel_pos = object->position - camera->cameraPos;
                glm::vec3 compressedPosition = compressSqrt(rel_pos, zoomFactor);
                // model = glm::translate(model, object_ptr.first->position / zoomFactor); // Linear scaling
                // model = glm::translate(model, compressSqrt(object_ptr.first->position, zoomFactor)); // Sqrt scaling
                model = glm::translate(model, compressedPosition); // relative camera positioning w/ sqrt
                // model = glm::scale(model, glm::vec3(radius, radius, radius) / (float)pow(zoomFactor, 1.10f));
                // model = glm::scale(model, glm::vec3(radius, radius, radius) / zoomFactor);
                model = glm::scale(model, compressSqrt(glm::vec3(radius, radius, radius), pow(zoomFactor, 1.05)));
                // shader->set_model(model);

                use_vertex(model, camera->view, camera->perspective_projection);

                glDrawElements(GL_TRIANGLES, object->NDC_indices.size(), GL_UNSIGNED_INT, 0);

                //////////////////
                // Trail rendering
                //////////////////

                // Set model uniform to identity matrix to stop line from moving with objects
                model = glm::mat4(1.0f);
                shader->set_model(model);

                // Update the trail buffer
                updateTrailBuffer(object.get(), zoomFactor);

                // Get and draw the trail VAO
                unsigned int trailVAO = object->trail_VAO; // Note: .second grabs the VAO from the pair value in the map
                glBindVertexArray(trailVAO);
                glDrawArrays(GL_LINE_STRIP, 0, object->trail_points.size());

                // 2D screen space renders
                // shader->set_projection(camera->ortho_projection);
                glm::vec4 clip = camera->perspective_projection * camera->view * glm::vec4(compressedPosition, 1.0f);
                // if (clip.w <= 0.0f) return;

                glm::vec3 ndc = glm::vec3(clip) / clip.w;

                // float screenX = (ndc.x * 0.5f + 0.5f) * SCR_WIDTH;
                // float screenY = (ndc.y * 0.5f + 0.5f) * SCR_HEIGHT;
                float screenX = 100;
                float screenY = 100;

                use_billboard(camera->ortho_projection, 10, glm::vec2(screenX, screenY));

                glBindVertexArray(object->billboard_VAO);
                glDrawArrays(GL_TRIANGLE_FAN, 0, object->billboard_coordinates.size() / 3);
            }
        }

        glm::vec3 compressSqrt(const glm::vec3& pos, float scale) {
            float r = glm::length(pos);
            if (r == 0.0f) return pos;
            float r_c = sqrt(r) * scale;
            return glm::normalize(pos) * r_c;
        }

        /**
         * Method that updates the buffers present in line_map to reflect the current line points
         */
        void updateTrailBuffer(const CelestialObject* object, float zoomFactor) {
            unsigned int trailVBO = object->trail_VBO;
            std::vector<glm::vec3> zoomed_points;

            std::vector<glm::vec3> trail_pts = object->trail_points.trail_points;
            for (const auto& position_vec : trail_pts) {
                zoomed_points.push_back(position_vec / zoomFactor);
            }

            // Pushes data to buffer, VAO automatically picks it up
            glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
            glBufferData(GL_ARRAY_BUFFER,
                object->trail_points.size() * sizeof(glm::vec3),
                zoomed_points.data(),
                GL_DYNAMIC_DRAW);
        }

        /**
         * This function activates the Shader program from the Shader object
         * This function sets the uniforms for view and projection in the Shader class
         * Does this function need to update the matrices before using them to update the uniforms? I think so.
         * @deprecated Replaced with use_vertex() and use_billboard()
         * @see use_vertex() & use_billboard()
         */
        void shader_setup() {
            shader->use();
            shader->set_view(camera->view);
            shader->set_projection(camera->perspective_projection);
        }

        void use_vertex(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
            shader->use_vertex(model, view, projection);
        }

        void use_billboard(glm::mat4 ortho, float billboardSize, glm::vec2 screenPosition) {
            shader->use_billboard(ortho, billboardSize, screenPosition);
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