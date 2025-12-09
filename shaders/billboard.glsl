#version 330

layout (location = 0) in vec2 aPos;

uniform mat4 ortho;
uniform float billboardSize; // In pixels
uniform vec2 screenPosition; // In pixels

out vertColor;

void main() {
    vec2 centered = (aPos + vec2(0.5)) * billboardSize; //

    vec2 worldPos = screenPosition + centered;

    gl_Position = ortho * vec4(worldPos, 0.0, 1.0);
}
