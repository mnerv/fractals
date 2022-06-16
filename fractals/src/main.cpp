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
#include "mono/mono.hpp"
#include "glad/glad.h"

namespace nrv {
struct vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

auto read_text(std::string const& filename) -> std::string {
    std::ifstream input{filename, std::ios::in};
    if (!input.is_open() || input.fail())
        throw std::runtime_error("ERROR: Loading textfile!");
    return {
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };
}

template <typename T, std::size_t N>
constexpr auto length_of(T (&)[N]) -> std::size_t {
    return N;
}

struct keystate {
    mno::key key;
    bool states[2]{false, false};

    keystate(mno::key k) : key{k} {}
    auto update(bool const& state) -> void {
        states[1] = states[0];
        states[0] = state;
    }
    inline auto pressed() const -> bool {return states[0]; }
    inline auto releaseed() const -> bool { return !states[1]; }
    inline auto clicked() const -> bool {return states[0] && !states[1]; }

    inline static auto make(mno::key k) -> mno::local<keystate> {
        return mno::make_local<keystate>(k);
    }
};
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    mno::window window{};
    window.set_position(window.xpos(), -800);

    auto graphics = window.graphics_context();

    nrv::vertex vertices[] {
        {{-1.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{ 1.0f,  1.0f,  0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{ 1.0f, -1.0f,  0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    };
    std::uint32_t indices[] {
        0, 1, 2,
        0, 2, 3
    };

    auto load_shader = [] {
        return mno::shader::make(
            nrv::read_text("./shaders/410.shader.gl.vert"),
            nrv::read_text("./shaders/410.koch3d.gl.frag")
        );
    };
    auto shader = load_shader();

    mno::array_buffer array_buffer{};
    array_buffer.add_vertex_buffer(mno::vertex_buffer::make(vertices, sizeof(vertices), {
        {mno::shader::type::vec3, "a_position"},
        {mno::shader::type::vec4, "a_color"},
        {mno::shader::type::vec2, "a_uv"},
    }));
    array_buffer.set_index_buffer(mno::index_buffer::make(indices, sizeof(indices), nrv::length_of(indices)));

    auto width  = window.buffer_width();
    auto height = window.buffer_height();
    //auto buffer = mno::make_local<mno::framebuffer>(width, height);

    std::random_device rdev;
    std::mt19937 rng{rdev()};  // pass rdev into as seed
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1);

    mno::image noise_image{width, height};
    auto generate_noise = [&] {
        for (auto i = 0; i < noise_image.height(); i++) {
            for (auto j = 0; j < noise_image.width(); j++) {
                auto data = mno::u8(dist(rng) * 255);
                noise_image.set(j, i, data, data, data);
            }
        }
    };
    generate_noise();
    auto noise_texture = mno::make_ref<mno::texture>(noise_image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    auto current_time = window.time();
    auto last_time    = current_time;
    [[maybe_unused]]auto delta_time   = current_time - last_time;
    auto is_running   = true;

    auto key_down = [&](mno::event const& event) {
        auto e = static_cast<mno::key_down_event const&>(event);
        if (e.key() == mno::key::Q)
            is_running = false;
    };
    auto key_up = [&](mno::event const& event) {
        auto e = static_cast<mno::key_down_event const&>(event);
        if (e.key() == mno::key::R) {
            try {
                shader = load_shader();
                spdlog::info("Reload shader");
            } catch(std::runtime_error const& e) {
                spdlog::error(e.what());
            }
        }
    };
    window.add_event_listener(mno::event_type::key_down, key_down);
    window.add_event_listener(mno::event_type::key_up, key_up);

    mno::f64 mouse_posx, mouse_posy;
    while (is_running) {
        last_time    = current_time;
        current_time = window.time();
        delta_time   = current_time - last_time;
        is_running   = !window.shouldclose();
        window.buffer_size(width, height);
        window.mouse_pos(mouse_posx, mouse_posy);

        // OUTPUT TO SCREEN PASS
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->bind();
        shader->num("u_time", mno::f32(window.time()));
        shader->vec2("u_resolution", {width, height});
        shader->vec2("u_mouse", {mouse_posx, mouse_posy});

        graphics->draw_triangles(array_buffer);

        window.swap();
        window.poll();
    }

    return 0;
}

