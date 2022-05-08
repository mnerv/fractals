#version 410 core

layout(location = 0) out vec4 color;

in vec4 io_color;
in vec2 io_uv;

uniform sampler2D u_render;

void main() {
    color = texture(u_render, io_uv);
}

