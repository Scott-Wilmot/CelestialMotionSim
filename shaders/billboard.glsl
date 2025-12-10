#version 330

layout (location = 0) in vec2 aPos;

uniform mat4 ortho; // View matrix equivalent
uniform mat4 screenPosition; // Model matrix equivalent
uniform vec3 color;

out vec3 vertColor;

void main() {
    gl_Position = ortho * screenPosition * vec4(aPos, 0.0f, 1.0f);
    vertColor = color;
}
