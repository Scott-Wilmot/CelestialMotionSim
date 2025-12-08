#ifndef OPENGLPRACTICE_SHADER_H
#define OPENGLPRACTICE_SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int vertexProgram, billboardProgram;

    // Shader Uniform Locations for vertexProgram
    int modelLocation, viewLocation, projectionLocation;

    // Shader Uniform Locations for billboardProgram
    int orthoLocation, billboardSizeLocation, screenPositionLocation;

    Shader(const char* vertexPath, const char* fragmentPath, const char* billboardPath, float aspect) {
        // File and data objects
        std::string vertexCode, fragmentCode, billboardCode;
        std::ifstream vertexFile,fragmentFile, billboardFile;
        try {
            vertexFile.open(vertexPath);
            fragmentFile.open(fragmentPath);
            billboardFile.open(billboardPath);
            // File contents into stream
            std::stringstream vertexStream, fragmentStream, billboardStream;
            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();
            billboardStream << billboardFile.rdbuf();
            // Close file instances
            vertexFile.close();
            fragmentFile.close();
            billboardFile.close();
            // Convert strings to stream
            vertexCode = vertexStream.str();
            fragmentCode = fragmentStream.str();
            billboardCode = billboardStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_IO_ERROR" << std::endl;
        }

        const char* vertexShaderCode = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();
        const char* billboardShaderCode = billboardCode.c_str();

        // Compile shaders
        unsigned int vertex, fragment, billboard;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        billboard = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex, 1, &vertexShaderCode, NULL);
        glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
        glShaderSource(billboard, 1, &billboardShaderCode, NULL);
        glCompileShader(vertex);
        glCompileShader(fragment);
        glCompileShader(billboard);

        vertexProgram = glCreateProgram();
        glAttachShader(vertexProgram, vertex);
        glAttachShader(vertexProgram, fragment);
        glLinkProgram(vertexProgram);

        billboardProgram = glCreateProgram();
        glAttachShader(billboardProgram, billboard);
        glAttachShader(billboardProgram, fragment);
        glLinkProgram(billboardProgram);

        // Error checking
        checkCompileErrors(vertex, "VERTEX");
        checkCompileErrors(fragment, "FRAGMENT");
        checkCompileErrors(vertexProgram, "PROGRAM");

        // Delete shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(billboard);

        // Shader uniform initialization
        modelLocation = glGetUniformLocation(vertexProgram, "model");
        viewLocation = glGetUniformLocation(vertexProgram, "view");
        projectionLocation = glGetUniformLocation(vertexProgram, "projection");

        orthoLocation = glGetUniformLocation(billboardProgram, "ortho");
        billboardSizeLocation = glGetUniformLocation(billboardProgram, "billboardSize");
        screenPositionLocation = glGetUniformLocation(billboardProgram, "screenPosition");

        aspect_ratio = aspect;
    }

    void use() {
        glUseProgram(vertexProgram);
    }

    void use_vertex(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        glUseProgram(vertexProgram);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void use_billboard(glm::mat4 ortho, float billboardSize, glm::vec2 screenPosition) {
        glUseProgram(billboardProgram);
        glUniform4fv(orthoLocation, 1, glm::value_ptr(ortho));
        glUniform1f(billboardSizeLocation, billboardSize);
        glUniform2fv(screenPositionLocation, 1, glm::value_ptr(screenPosition));
    }

    // Shader Uniform Setters
    void set_model(glm::mat4 model) {
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    }
    void set_view(glm::mat4 view) {
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    }
    void set_projection(glm::mat4 projection) {
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    }


    private:
        float aspect_ratio;

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

#endif //OPENGLPRACTICE_SHADER_H