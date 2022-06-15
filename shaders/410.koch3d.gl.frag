// https://www.shadertoy.com/view/WtGXDD
#version 410 core
layout(location = 0) out vec4 color;

#define MAX_STEPS        100
#define MAX_DISTANCE     100.0
#define SURFACE_DISTANCE 0.001
#define TWO_PI           6.283185307179586
#define PI               3.141592653589793

in vec4 io_color;
in vec2 io_uv;
uniform float     u_time;
uniform vec2      u_resolution;
uniform vec2      u_mouse;
uniform sampler2D u_texture;

mat2 rot(float a) {
    float s = sin(a), c = cos(a);
    return mat2(c, -s, s, c);
}

float sd_box(vec3 point, vec3 size) {
    point = abs(point) - size;
    return length(max(point, 0.0)) + min(max(point.x, max(point.y, point.z)), 0.0);
}

// Angle to vector
vec2 N(float angle) {
    return vec2(sin(angle), cos(angle));
}

vec2 koch(vec2 uv) {
    uv.x = abs(uv.x);

    vec3 col = vec3(0.0);
    float d;

    float angle = 0;
    vec2 n = N((5.0 / 6.0) * PI);

    uv.y += tan((5.0 / 6.0) * PI) * 0.5;
    d     = dot(uv - vec2(0.5, 0.0), n);
    uv   -= max(0.0, d) * n * 2.0;

    float scale = 1.0;

    n = N(2.0 / 3.0 * PI);
    uv.x += 0.5;
    for (int i = 0; i < 4; i++) {
        uv    *= 3.0;
        scale *= 3.0;
        uv.x  -= 1.5;

        uv.x  = abs(uv.x);
        uv.x -= 0.5;
        d     = dot(uv, n);
        uv   -= min(0.0, d) * n * 2.0;
    }
    uv /= scale;
    return uv;
}

float get_dist(vec3 point) {
    float d = sd_box(point, vec3(1));

    vec2 xy = koch(point.xy);
    vec2 yz = koch(point.yz);
    vec2 xz = koch(point.xz);

    //d = xy.y;
    d = max(xy.y, max(yz.y, xz.y));
    //d = max(d, abs(point.z) - 0.1);

    return d;
}

float ray_march(vec3 ray_origin, vec3 ray_direction) {
    float distance = 0.0;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 point = ray_origin + ray_direction * distance;
        float d_s = get_dist(point);
        distance  += d_s;
        if (distance > MAX_DISTANCE || abs(d_s) < SURFACE_DISTANCE) break;
    }
    return distance;
}

vec3 get_normal(vec3 point) {
    float d = get_dist(point);
    vec2 e  = vec2(0.001, 0);
    vec3 n  = d - vec3(
            get_dist(point - e.xyy),
            get_dist(point - e.yxy),
            get_dist(point - e.yyx)
        );
    return normalize(n);
}

vec3 get_ray_direction(vec2 uv, vec3 point, vec3 length, float z) {
    vec3 f = normalize(length - point);
    vec3 r = normalize(cross(vec3(0, 1, 0), f));
    vec3 u = cross(f, r);
    vec3 c = f * z;
    vec3 i = c + uv.x * r + uv.y * u;
    vec3 d = normalize(i);
    return d;
}

void main() {
    vec2 uv = (io_uv - 0.5) * u_resolution / u_resolution.y;
    vec2 m  = u_mouse.xy / u_resolution.xy;

    vec3 ray_origin = vec3(0, 3, -3);
    ray_origin.yz *= rot(-m.y * TWO_PI + 1.0);
    ray_origin.xz *= rot(-m.x * TWO_PI);

    vec3 ray_direction = get_ray_direction(uv, ray_origin, vec3(0, 0, 0), 3.0);
    vec3 col = vec3(0.0);
    float d  = ray_march(ray_origin, ray_direction);

    if (d < MAX_DISTANCE) {
        vec3 p = ray_origin + ray_direction * d;
        vec3 n = get_normal(p);
        vec3 r = reflect(ray_direction, n);

        float dif = dot(n, normalize(vec3(1, 2, 3))) * 0.5 + 0.5;
        col = vec3(dif);

        col = n * .5 + .5;
    }
    //col *= 0.0;

    //vec2 st = koch(uv) * 4.0;
    //col = vec3(max(st.y, 0.0));
    col.rgb = pow(col.rgb, vec3(0.4545));
    color = vec4(col, 1.0);
}

