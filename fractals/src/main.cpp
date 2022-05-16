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

namespace nrv {
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
}  // namespace nrv

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
    window.set_position(window.xpos(), 200);

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
        auto vertex_shader   = "./shaders/410.shader.gl.vert";
        auto fragment_shader = "./shaders/410.conway.gl.frag";
        return mno::shader::make(nrv::read_text(vertex_shader),
                                  nrv::read_text(fragment_shader));
    };

    auto shader = mno::shader::make(
        nrv::read_text("./shaders/410.shader.gl.vert"),
        nrv::read_text("./shaders/410.conway_post.gl.frag")
    );
    auto conway_shader  = load_shader();
    auto texture_shader = mno::shader::make(
        nrv::read_text("./shaders/410.shader.gl.vert"),
        nrv::read_text("./shaders/410.texture.gl.frag")
    );

    mno::array_buffer array_buffer{};
    array_buffer.add_vertex_buffer(mno::vertex_buffer::make(vertices, sizeof(vertices), {
        {mno::shader::type::vec3, "a_position"},
        {mno::shader::type::vec4, "a_color"},
        {mno::shader::type::vec2, "a_uv"},
    }));
    array_buffer.set_index_buffer(mno::index_buffer::make(indices, sizeof(indices), sizeof(indices) / sizeof(std::uint32_t)));

    auto width  = window.buffer_width();
    auto height = window.buffer_height();

    auto buffer_a = mno::make_local<mno::framebuffer>(width, height);
    auto buffer_b = mno::make_local<mno::framebuffer>(width, height);

    std::uint32_t frame = 0;

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

    std::vector<mno::ref<nrv::keystate>> keys{};
    auto update_keystates = [&] {
        std::for_each(std::begin(keys), std::end(keys), [&](auto& keystate) {
            keystate->update(window.keystate(keystate->key) == mno::keystate::PRESS);
        });
    };
    auto make_keystate = [&](mno::key const& key) {
        keys.emplace_back(nrv::keystate::make(key));
        return keys.back();
    };

    auto current_time = window.time();
    auto last_time    = current_time;
    auto delta_time   = current_time - last_time;

    mno::f32 pan_speed  = 0.15f;
    mno::f32 zoom_speed = 0.15f;

    mno::f32 zoom_dv = 0.0f;
    mno::f32 zoom    = 1.0f;
    glm::vec2 velocity{0.0, 0.0};
    glm::vec2 location{0.0, 0.0};
    auto is_running   = true;
    auto is_sim_pause = true;

    auto move_left  = make_keystate(mno::key::A);
    auto move_right = make_keystate(mno::key::D);
    auto move_up    = make_keystate(mno::key::W);
    auto move_down  = make_keystate(mno::key::S);
    auto move_reset = make_keystate(mno::key::C);

    auto zoom_reset = make_keystate(mno::key::N0);
    auto zoom_in    = make_keystate(mno::key::F);
    auto zoom_out   = make_keystate(mno::key::G);

    auto play_pause = make_keystate(mno::key::SPACE);
    auto step_sim   = make_keystate(mno::key::M);

    auto simulate = [&] {
        // FIRST PASS - Conway's Game of Life
        buffer_a->resize(width, height);
        buffer_a->bind();

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        conway_shader->bind();
        conway_shader->num("u_time", float(window.time()));
        conway_shader->vec2("u_res", {width, height});
        conway_shader->num("u_frame", frame);
        conway_shader->num("u_texture", 0);
        noise_texture->bind(0);

        conway_shader->num("u_texture1", 1);
        buffer_b->texture()->bind(1);

        array_buffer.bind();
        array_buffer.vertex_buffer()->bind();
        array_buffer.index_buffer()->bind();
        glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);
        buffer_a->unbind();

        // SECOND PASS - STORE LAST COMPUTATION
        buffer_b->resize(width, height);
        buffer_b->bind();
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        texture_shader->bind();
        texture_shader->num("u_texture", 0);
        buffer_a->texture()->bind(0);

        array_buffer.bind();
        array_buffer.vertex_buffer()->bind();
        array_buffer.index_buffer()->bind();
        glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);
        buffer_b->unbind();
        frame++;
    };
    simulate(); // run once to initialize buffers

    auto reload_shader = [&] {
        noise_image.resize(width, height);
        generate_noise();
        noise_texture->set_image(noise_image);
        try {
            conway_shader = load_shader();
        } catch (std::runtime_error const& e) {
            spdlog::error(e.what());
        }
        frame = 0;
        simulate();
    };

    auto key_down = [&](mno::event const& event) {
        auto e = static_cast<mno::key_down_event const&>(event);
        if (e.key() == mno::key::Q)
            is_running = false;
    };
    auto key_up = [&](mno::event const& event) {
        auto e = static_cast<mno::key_up_event const&>(event);
        if (e.key() == mno::key::R) {
            spdlog::info("reload shader");
            reload_shader();
        }
    };
    window.add_event_listener(mno::event_type::key_down, key_down);
    window.add_event_listener(mno::event_type::key_up, key_up);

    while (is_running) {
        last_time    = current_time;
        current_time = window.time();
        delta_time   = current_time - last_time;

        is_running = !window.shouldclose();
        width  = window.buffer_width();
        height = window.buffer_height();
        update_keystates();

        // handle input
        if (move_left->pressed())
            velocity.x = -pan_speed;
        else if (move_right->pressed())
            velocity.x = pan_speed;
        else
            velocity.x = 0.0f;

        if (move_up->pressed())
            velocity.y = pan_speed;
        else if (move_down->pressed())
            velocity.y = -pan_speed;
        else
            velocity.y = 0.0f;

        if (zoom_in->pressed())
            zoom_dv = -zoom_speed;
        else if (zoom_out->pressed())
            zoom_dv = zoom_speed;
        else
            zoom_dv = 0.0f;

        if (zoom_reset->clicked())
            zoom = 1.0f;
        if (move_reset->clicked())
            location = {0.0f, 0.0f};

        if (play_pause->clicked())
            is_sim_pause = !is_sim_pause;
        if (step_sim->clicked())
            simulate();

        // update movement
        location += velocity * mno::f32(delta_time);
        zoom += zoom_dv * mno::f32(delta_time);

        // FIRST PASS
        if (!is_sim_pause)
            simulate();

        // OUTPUT TO SCREEN PASS
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->bind();
        shader->vec2("u_res", {width, height});
        shader->vec2("u_location", location);
        shader->num("u_zoom", zoom);
        shader->num("u_texture", 0);
        buffer_b->texture()->bind(0);

        array_buffer.bind();
        array_buffer.vertex_buffer()->bind();
        array_buffer.index_buffer()->bind();
        glDrawElements(GL_TRIANGLES, array_buffer.index_buffer()->count(), GL_UNSIGNED_INT, nullptr);

        window.swap();
        window.poll();
    }

    return 0;
}

