#ifndef OPENGLPRACTICE_SHADER_H
#define OPENGLPRACTICE_SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    // Shader Uniform Locations
    int modelLocation, viewLocation, projectionLocation;

    virtual ~Shader() = 0;

    Shader(const char* vertexPath, const char* fragmentPath, float aspect) {
        // File and data objects
        std::string vertexCode, fragmentCode;
        std::ifstream vertexFile,fragmentFile;
        try {
            vertexFile.open(vertexPath);
            fragmentFile.open(fragmentPath);
            // File contents into stream
            std::stringstream vertexStream, fragmentStream;
            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();
            // Close file instances
            vertexFile.close();
            fragmentFile.close();
            // Convert strings to stream
            vertexCode = vertexStream.str();
            fragmentCode = fragmentStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_IO_ERROR" << std::endl;
        }

        const char* vertexShaderCode = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();

        // Compile shaders
        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex, 1, &vertexShaderCode, NULL);
        glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
        glCompileShader(vertex);
        glCompileShader(fragment);

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        // Error checking
        checkCompileErrors(vertex, "VERTEX");
        checkCompileErrors(fragment, "FRAGMENT");
        checkCompileErrors(ID, "PROGRAM");

        // Delete shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        // Shader uniform initialization
        modelLocation = glGetUniformLocation(ID, "model");
        viewLocation = glGetUniformLocation(ID, "view");
        projectionLocation = glGetUniformLocation(ID, "projection");

        aspect_ratio = aspect;
    }

    void use() {
        glUseProgram(ID);
    }

    // Uniform utility functions

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

class Shader2D : public Shader {
    public:
        Shader2D(const char *vertexPath, const char *fragmentPath, float aspect, float ortho)
            : Shader(vertexPath, fragmentPath, aspect)
        {
            orthographic_bound = ortho;
        }



    private:
        // Ortho parameters
        float orthographic_bound;
};

class Shader3D : public Shader {
    public:
        Shader3D(const char *vertexPath, const char *fragmentPath, float* aspect)
            : Shader(vertexPath, fragmentPath, aspect)
        {

        }

};

#endif //OPENGLPRACTICE_SHADER_H