#version 450

out vec2 uv;

void main(void) {
    float x = float((gl_VertexID & 1) << 2);
    float y = float((gl_VertexID & 2) << 1);
    uv = 0.5 * vec2(x, y);
    gl_Position = vec4(x - 1.0, y - 1.0, 0, 1);
}
