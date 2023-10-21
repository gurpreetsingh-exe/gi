#version 450

in vec3 pos;
out vec4 color;

void main() {
    vec3 normal = normalize(cross(dFdy(pos), dFdx(pos)));
    vec3 light_pos = vec3(10.0);
    float diffuse = 0.5 + 0.5 * dot(normalize(light_pos - pos), normal);
    color = vec4(vec3(1.0f) * diffuse, 1.0f);
}
