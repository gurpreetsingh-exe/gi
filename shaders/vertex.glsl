#version 450

layout (location = 0) in vec3 position;
out vec3 pos;

uniform mat4 view_projection;

void main() {
    gl_Position = view_projection * vec4(position, 1.0f);
    pos = position;
}
