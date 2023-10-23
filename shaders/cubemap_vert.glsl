#version 450

layout (location = 0) in vec3 position;
out vec3 pos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    pos = position;
    mat4 mat = projection * view;
    mat[3] = vec4(0.0, 0.0, 0.0, 0.1);
    vec4 pos = mat * vec4(position, 1.0);
    gl_Position = pos.xyww;
}
