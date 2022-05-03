#version 410 core
layout(location = 0) out vec4 o_color;

#define PI 3.14159265359

in vec4 io_color;
in vec2 io_uv;

uniform float u_time;
uniform vec2  u_res;
uniform vec2  u_mouse;
uniform vec4  u_color;
uniform sampler2D u_texture;

void main() {
    vec2 uv = (io_uv - 0.5) * u_res / u_res.y;
    vec3 color = vec3(1.0);

    float x = 2.0 * PI * 0.5 * u_time;
    //color = vec3(length(uv + vec2(cos(x), sin(x))));
    color = vec3(length(uv));

    o_color = vec4(color, 1.0);
}

