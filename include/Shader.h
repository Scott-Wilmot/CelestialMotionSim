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

    Shader(const char* vertexPath, const char* fragmentPath) {
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
    }

    void use() {
        glUseProgram(ID);
    }

    // Uniform utility functions

private:
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