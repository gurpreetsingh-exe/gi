#version 450

layout (location = 0) in vec3 position;
out vec3 pos;

uniform mat4 modelViewProjection;
uniform mat4 transform;

void main() {
    gl_Position = modelViewProjection * transform * vec4(position, 1.0f);
    pos = position;
}
