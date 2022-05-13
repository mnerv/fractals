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
#include "framebuffer.hpp"
#include "event.hpp"

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
    //window.set_position(window.xpos(), 200);

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
        auto vertex_shader   = "./shaders/410.shader.gl.vert";
        auto fragment_shader = "./shaders/410.conway.gl.frag";
        return mono::shader::make(mono::read_text(vertex_shader),
                                  mono::read_text(fragment_shader));
    };

    auto shader = load_shader();
    auto render_shader = mono::shader::make(mono::read_text("./shaders/410.shader.gl.vert"),
                                            mono::read_text("./shaders/410.render.gl.frag"));
    auto post_shader = mono::shader::make(mono::read_text("./shaders/410.shader.gl.vert"),
                                          mono::read_text("./shaders/410.post.gl.frag"));

    mono::array_buffer array_buffer{};
    array_buffer.add_vertex_buffer(mono::vertex_buffer::make(vertices, sizeof(vertices), {
        {mono::shader::type::vec3, "a_position"},
        {mono::shader::type::vec4, "a_color"},
        {mono::shader::type::vec2, "a_uv"},
    }));
    array_buffer.set_index_buffer(mono::index_buffer::make(indices, sizeof(indices), sizeof(indices) / sizeof(std::uint32_t)));

    auto width  = window.buffer_width();
    auto height = window.buffer_height();

    mono::framebuffer buffer_a{width, height};
    mono::framebuffer buffer_b{width, height};

    auto reload_key = window.make_key(GLFW_KEY_R);
    std::uint32_t frame = 0;

    std::random_device rdev;
    std::mt19937 rng{rdev()};  // pass rdev into as seed
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    mono::image noise_image{width, height};
    auto generate_noise = [&] {
        for (auto i = 0; i < noise_image.height(); i++) {
            for (auto j = 0; j < noise_image.width(); j++) {
                auto data = std::uint8_t (dist(rng));
                noise_image.set(j, i, data, data, data);
            }
        }
    };
    generate_noise();
    auto noise_texture = mono::make_ref<mono::texture>(noise_image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    auto current_time = window.time();
    auto last_time    = current_time;
    auto delta_time   = current_time - last_time;

    auto zoom_in    = window.make_key(GLFW_KEY_F);
    auto zoom_out   = window.make_key(GLFW_KEY_G);

    auto left_key   = window.make_key(GLFW_KEY_A);
    auto right_key  = window.make_key(GLFW_KEY_D);
    auto up_key     = window.make_key(GLFW_KEY_W);
    auto down_key   = window.make_key(GLFW_KEY_S);

    float zoom       = 1.0f;
    float zoom_speed = 0.15f;
    float pan_speed  = 0.10f;
    glm::vec2 location{0.0, 0.0};
    auto is_running   = true;
    auto is_sim_pause = false;

    auto key_down = [&](mono::event const& event) {
        auto e = static_cast<mono::key_down_event const&>(event);
        if (e.key() == GLFW_KEY_Q)
            is_running = false;
        if (e.key() == GLFW_KEY_SPACE)
            is_sim_pause = true;
    };
    auto key_up = [&](mono::event const& event) {
        auto e = static_cast<mono::key_up_event const&>(event);
        if (e.key() == GLFW_KEY_SPACE)
            is_sim_pause = false;
    };
    window.add_event_listener(mono::event_type::key_down, key_down);
    window.add_event_listener(mono::event_type::key_up, key_up);

    while (is_running) {
        last_time    = current_time;
        current_time = window.time();
        delta_time   = current_time - last_time;

        is_running = !window.shouldclose();
        width  = window.buffer_width();
        height = window.buffer_height();

        using namespace mono::state;
        if (key::is_clicked(reload_key)) {
            spdlog::info("reload shader");

            noise_image.resize(width, height);
            generate_noise();
            noise_texture = mono::make_ref<mono::texture>(noise_image);
            glGenerateMipmap(GL_TEXTURE_2D);
            try {
                shader = load_shader();
            } catch (std::runtime_error const& e) {
                spdlog::error(e.what());
            }
            frame = 0;
        }
        if (key::is_press(zoom_in)) {
            zoom -= zoom_speed * float(delta_time);
        }
        if (key::is_press(zoom_out)) {
            zoom += zoom_speed * float(delta_time);
        }
        if (key::is_press(right_key)) {
            location.x += pan_speed * float(delta_time);
        } else if (key::is_press(left_key)) {
            location.x -= pan_speed * float(delta_time);
        }

        if (key::is_press(up_key)) {
            location.y += pan_speed * float(delta_time);
        } else if (key::is_press(down_key)) {
            location.y -= pan_speed * float(delta_time);
        }

        // FIRST PASS
        if (!is_sim_pause) {
            buffer_a.resize(width, height);
            buffer_a.bind();

            glViewport(0, 0, width, height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shader->bind();
            shader->num("u_time", float(window.time()));
            shader->vec2("u_res", {width, height});
            shader->num("u_frame", frame);
            shader->num("u_texture", 0);
            noise_texture->bind(0);

            shader->num("u_texture1", 1);
            buffer_b.texture()->bind(1);

            array_buffer.bind();
            array_buffer.vertex_buffer()->bind();
            array_buffer.index_buffer()->bind();
            glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);
            buffer_a.unbind();
            frame++;
        }

        // SECOND PASS - STORE LAST COMPUTATION
        buffer_b.resize(width, height);
        buffer_b.bind();
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        render_shader->bind();
        render_shader->num("u_texture", 0);
        buffer_a.texture()->bind(0);

        array_buffer.bind();
        array_buffer.vertex_buffer()->bind();
        array_buffer.index_buffer()->bind();
        glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);
        buffer_b.unbind();

        // OUTPUT TO SCREEN PASS
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        post_shader->bind();
        post_shader->vec2("u_res", {width, height});
        post_shader->vec2("u_location", location);
        post_shader->num("u_zoom", zoom);
        post_shader->num("u_texture", 0);
        buffer_b.texture()->bind(0);

        array_buffer.bind();
        array_buffer.vertex_buffer()->bind();
        array_buffer.index_buffer()->bind();
        glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);

        window.swap();
        window.poll();
    }

    return 0;
}

