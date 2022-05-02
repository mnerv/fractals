#version 410 core

layout(location = 0) out vec4 color;

in vec4 io_color;
in vec2 io_uv;

uniform vec4 u_color;
uniform float u_time;
uniform vec2 u_resolution;
uniform sampler2D u_texture;

void main() {
    // shader program here
    color = io_color;
}
