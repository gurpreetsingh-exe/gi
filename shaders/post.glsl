#version 450

in vec2 uv;
uniform sampler2D u_buf;

out vec4 color;

void main() {
    vec4 c = texture(u_buf, uv);
    color = c;
}

