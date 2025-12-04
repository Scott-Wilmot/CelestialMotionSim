#Boilerplate
cmake_minimum_required(VERSION 3.30.5)
project(OpenGLPractice)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLFW/GLAD source
add_subdirectory(external/glfw-3.4)
add_subdirectory(external/glad)

# Bring OpenGL system library into context
find_package(OpenGL REQUIRED)

# Include Shader class directory
include_directories(include)

# Target nlohmann
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE external)

# Define executable names
set (EXECUTABLES
        main
        test
)

# Create, link, and include for each executable file
foreach (exec ${EXECUTABLES})
    add_executable(${exec} src/sim/${exec}.cpp)
    target_link_libraries(${exec} PRIVATE glad glfw OpenGL::GL nlohmann_json)
    target_include_directories(${exec} PRIVATE external/glfw-3.4/include)
    target_include_directories(${exec} PRIVATE external/GLM-1.0.1)
endforeach()