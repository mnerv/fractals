#version 410 core
layout(location = 0) out vec4 o_color;

#define PI 3.14159265359

in vec4 io_color;
in vec2 io_uv;

uniform uint  u_frame;
uniform float u_time;
uniform vec2  u_res;
uniform vec2  u_mouse;
uniform vec4  u_color;
uniform sampler2D u_texture;
uniform sampler2D u_texture1;

int get_neighbors(ivec2 p) {
    int num = 0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (y == 0 && x == 0) continue;
            num += texelFetch(u_texture1, p + ivec2(x, y), 0).r > 0.5 ? 1 : 0;
        }
    }
    return num;
}

void main() {
    vec2 uv = (io_uv - 0.5) * u_res / u_res.y;
    vec4 color = vec4(0.0);
    if (u_frame < 1) {
        color = texture(u_texture, io_uv);
        o_color = color;
        return;
    }
    bool alive = texelFetch(u_texture1, ivec2(io_uv * u_res), 0).r > 0.5;
    int num = get_neighbors(ivec2(io_uv * u_res));
    int next = (alive && num == 2 || num == 3) ? 1 : 0;
    color = vec4(next);
    o_color = color;
}

