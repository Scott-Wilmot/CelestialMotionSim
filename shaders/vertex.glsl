#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 cameraPos;
uniform vec3 worldPos;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertColor;

vec3 compress(vec3 relativePos, float zoom) {
    float r = length(worldPos);
    if (r == 0.0f) return worldPos;
    float root = sqrt(r) * zoom;
    return normalize(worldPos) * root;
}

void main() {
    vec3 relativePos = worldPos - cameraPos;
    vec3 compressedPosition = compress(relativePos);

    gl_Position = projection * view * vec4(compressedPosition, 1.0f) * vec4(aPos, 1.0);
    vertColor = vec3(1.0, 0.0, 1.0);
}
