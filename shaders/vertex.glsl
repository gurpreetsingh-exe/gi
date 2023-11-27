#version 450

layout (location = 0) in vec3 position;
layout (std140) uniform Camera {
    mat4 view_projection;
};

out vec3 pos;

void main() {
    gl_Position = view_projection * vec4(position, 1.0f);
    pos = position;
}
