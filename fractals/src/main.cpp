/**
 * @file   main.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Fractals program
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <random>

#include "spdlog/spdlog.h"

#include "window.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace mono {
struct vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

auto read_text(std::string const& filename) -> std::string {
    std::ifstream input{filename, std::ios::in};
    if (!input.is_open() || input.fail()) return "";
    return {
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };
}
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    mono::window window{{
        "Fractals",
    }};
    window.set_position(window.xpos(), 200);

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

    auto load_shader = []{
        auto vertex_shader = "./shaders/410.shader.gl.vert";
        auto fragment_shader = "./shaders/410.conway.gl.frag";
        return mono::shader::make(mono::read_text(vertex_shader),
                                  mono::read_text(fragment_shader));
    };

    auto shader = load_shader();
    auto render_shader = mono::shader::make(mono::read_text("./shaders/410.shader.gl.vert"),
                                            mono::read_text("./shaders/410.render.gl.frag"));

    mono::array_buffer array_buffer{};
    mono::vertex_buffer vertex_buffer{vertices, sizeof(vertices)};
    mono::index_buffer index_buffer{indices, sizeof(indices)};

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mono::vertex), (void const*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    auto width  = window.buffer_width();
    auto height = window.buffer_height();
    mono::renderbuffer render_buffer{width, height};
    mono::texture render_texture{width, height};
    mono::framebuffer framebuffer{};
    framebuffer.texture(render_texture);
    framebuffer.render(render_buffer);

    mono::renderbuffer render_buffer_final{width, height};
    mono::texture render_texture_final{width, height};
    mono::framebuffer framebuffer_final{};
    framebuffer_final.texture(render_texture_final);
    framebuffer_final.render(render_buffer_final);


    auto quit_key   = window.make_key(GLFW_KEY_Q);
    auto reload_key = window.make_key(GLFW_KEY_R);
    std::uint32_t frame = 0;

    //std::random_device rdev{};
    std::mt19937 rng{1};  // pass rdev into as seed
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    mono::image noise_image{width, height};
    for (auto i = 0; i < noise_image.height(); i++) {
        for (auto j = 0; j < noise_image.width(); j++) {
            auto data = std::uint8_t(255.0 * dist(rng));
            noise_image.set(j, i, data, data, data);
        }
    }
    mono::texture noise_texture{noise_image};
    noise_texture.param(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    noise_texture.param(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    noise_texture.mipmap();

    auto is_running = true;
    while (is_running) {
        is_running = !window.shouldclose();
        width  = window.buffer_width();
        height = window.buffer_height();

        using namespace mono::state;
        if (key::is_clicked(quit_key))
            is_running = false;
        if (key::is_clicked(reload_key)) {
            spdlog::info("reload shader");
            try {
                shader = load_shader();
            } catch (std::runtime_error const& e) {
                spdlog::error(e.what());
            }
            frame = 0;
        }
        framebuffer.bind();
        render_texture.resize(width, height);
        render_buffer.resize(width, height);
        framebuffer.texture(render_texture);
        framebuffer.render(render_buffer);
        framebuffer.unbind();

        framebuffer_final.bind();
        render_texture_final.resize(width, height);
        render_buffer_final.resize(width, height);
        framebuffer_final.texture(render_texture_final);
        framebuffer_final.render(render_buffer_final);
        framebuffer_final.unbind();

        // FIRST PASS
        framebuffer.bind();

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->bind();
        shader->num("u_time", float(window.time()));
        shader->vec2("u_res", {width, height});
        shader->num("u_frame", frame);
        shader->num("u_texture", 0);
        noise_texture.bind(0);
        shader->num("u_texture1", 1);
        render_texture_final.bind(1);

        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);
        framebuffer.unbind();

        // STORE LAST PASS INTO ANOTHER BUFFER
        framebuffer_final.bind();
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        render_shader->bind();
        render_shader->num("u_texture", 0);
        render_texture.bind(0);

        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);
        framebuffer_final.unbind();

        // SECOND PASS
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        render_shader->bind();
        render_shader->num("u_texture", 0);
        render_texture.bind(0);

        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);

        window.swap();
        window.poll();
        frame++;
    }

    return 0;
}

