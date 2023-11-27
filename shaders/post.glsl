#version 450

in vec2 uv;
uniform sampler2D u_buf;
uniform sampler2D u_depth;
uniform bool u_motion_blur;

layout (std140) uniform Camera {
    mat4 view_projection;
    mat4 last_view_projection;
};

out vec4 color;

void main() {
    color = texture(u_buf, uv);
    if (u_motion_blur) {
        float depth = texture(u_depth, uv).x;
        vec3 current = vec3(uv * 2.f - 1.f, depth);
        vec4 prev = last_view_projection * (inverse(view_projection) * vec4(current, 1.0f));
        vec2 blur_vec = (current.xy - prev.xy) * 0.6f;
        int samples = 32;
        for (int i = 1; i < samples; ++i) {
            vec2 offset = blur_vec * (float(i) / float(samples - 1) - 0.5f);
            color += texture(u_buf, uv + offset);
        }
        color /= float(samples);
    }
}
