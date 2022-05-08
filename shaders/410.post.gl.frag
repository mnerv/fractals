#version 410 core

layout(location = 0) out vec4 color;

in vec4 io_color;
in vec2 io_uv;

uniform float u_zoom;
uniform vec2 u_location;
uniform vec2 u_res;
uniform sampler2D u_texture;

void main() {
    vec2 sample_location = (io_uv - 0.5) * u_zoom + u_location + 0.5;
    color = texture(u_texture, sample_location);

    if (sample_location.x * u_res.x < 0.0 || sample_location.x * u_res.x > u_res.x)
        color = vec4(0.0);
    if (sample_location.y * u_res.y < 0.0 || sample_location.y * u_res.y > u_res.y)
        color = vec4(0.0);
}

