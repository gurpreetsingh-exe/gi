#version 450

uniform samplerCube u_cubemap;

out vec4 FragColor;
in vec3 pos;

void main() {
    FragColor = texture(u_cubemap, pos);
}
