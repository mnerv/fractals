#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>

#include "spdlog/spdlog.h"

#include "window.hpp"
#include "shader.hpp"
#include "buffer.hpp"

namespace mono {
struct vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

auto basic_vertex_shader = R"(#version 410 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;

out vec4 io_color;
out vec2 io_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    io_color = a_color;
    io_uv    = a_uv;

    //gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0f);
    gl_Position = vec4(a_position, 1.0f);
}
)";

auto basic_fragment_shader = R"(#version 410 core
layout(location = 0) out vec4 o_color;

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
    color = vec3(length(uv));
    o_color = vec4(color, 1.0);
}
)";
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    mono::window window{{
        "Fractals",
    }};

    mono::vertex vertices[]{
        {{-1.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f,  0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{ 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    };
    std::uint32_t indices[]{
        0, 1, 2,
        0, 2, 3
    };

    mono::shader shader(mono::basic_vertex_shader, mono::basic_fragment_shader);
    mono::array_buffer array_buffer{};
    mono::vertex_buffer vertex_buffer{vertices, sizeof(vertices)};
    mono::index_buffer index_buffer{indices, sizeof(indices)};

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    auto is_running = true;
    while (is_running) {
        is_running = !window.shouldclose();

        if (window.get_key(GLFW_KEY_Q) == GLFW_PRESS)
            is_running = false;

        glViewport(0, 0, window.buffer_width(), window.buffer_height());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.vec2("u_res", {window.width(), window.height()});
        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);

        window.swap();
        mono::window::poll();
    }

    return 0;
}
