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
    window.set_position(window.x(), 200);

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
        auto fragment_shader = "./shaders/410.shader.gl.frag";
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

    std::uint32_t framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    std::uint32_t render_texture;
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.buffer_width(), window.buffer_height(), 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

    std::uint32_t render_buffer;
    glGenRenderbuffers(1, &render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.buffer_width(), window.buffer_height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("FRAMEBUFFER: Framebuffer is not complete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto quit_key   = window.make_key(GLFW_KEY_Q);
    auto reload_key = window.make_key(GLFW_KEY_R);

    auto is_running = true;
    while (is_running) {
        is_running = !window.shouldclose();
        auto width  = window.buffer_width();
        auto height = window.buffer_height();

        using namespace mono::state;
        if (key::is_clicked(quit_key))
            is_running = false;

        if (key::is_clicked(reload_key)) {
            spdlog::info("reload shader");
            shader = load_shader();
        }

        // TODO: Guard this to only run when resize happens
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindTexture(GL_TEXTURE_2D, render_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER: Framebuffer is not complete!\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // FIRST PASS
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->bind();
        shader->num("u_time", float(window.time()));
        shader->vec2("u_res", {width, height});
        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SECOND PASS
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        render_shader->bind();
        render_shader->num("u_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, render_texture);

        array_buffer.bind();
        vertex_buffer.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);

        window.swap();
        window.poll();
    }
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteBuffers(1, &render_texture);
    glDeleteRenderbuffers(1, &render_buffer);

    return 0;
}

