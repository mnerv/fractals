#version 410 core
layout(location = 0) out vec4 o_color;

in vec4 io_color;
in vec2 io_uv;
uniform sampler2D u_texture;

void main() {
    o_color = texture(u_texture, io_uv);
}
