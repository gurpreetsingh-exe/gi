#version 450

in vec3 pos;
layout (location = 0) out vec4 color;

uniform samplerCube u_cubemap;

void main() {
    color = vec4(pos, 1.0f);
    vec3 normal = normalize(cross(dFdy(pos), dFdx(pos)));
    vec3 c = texture(u_cubemap, -normal).rgb;
    vec3 light_pos = vec3(-10.0);
    float diffuse = 0.5 + 0.5 * dot(normalize(light_pos - pos), normal);
    color = vec4(c, 1.0f);
}
