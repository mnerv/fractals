#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace mono {
auto opengl_version() -> void {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

struct window_properties {
    std::string  title  = "no title";
    std::int32_t width  = 738;
    std::int32_t height = 480;
    std::int32_t xpos{INT32_MIN};
    std::int32_t ypos{INT32_MIN};
};

struct vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
};

class window {
  public:
    explicit window(window_properties const& props = {}) {
        if (!glfwInit()) throw std::runtime_error("Error initializing GLFW!");
        mono::opengl_version();
        m_data.title  = props.title;
        m_data.width  = props.width;
        m_data.height = props.height;
        m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
        if (m_window == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Error creating GLFW window!\n");
        }

        glfwGetWindowPos(m_window, &m_data.xpos, &m_data.ypos);
        if (props.xpos != INT32_MIN) m_data.xpos = props.xpos;
        if (props.ypos != INT32_MIN) m_data.ypos = props.ypos;
        glfwSetWindowPos(m_window, m_data.xpos, m_data.ypos);
        glfwMakeContextCurrent(m_window);
        if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
            glfwTerminate();
            throw std::runtime_error("Error failed to load glad!\n");
        }
        glfwSetWindowUserPointer(m_window, &m_data);
        glfwSetWindowSizeCallback(m_window,
        [](GLFWwindow* window, std::int32_t width, std::int32_t height) {
            auto data = mono::window::user_ptr(window);
            data->width  = width;
            data->height = height;
        });
        glfwSetFramebufferSizeCallback(m_window,
        [](GLFWwindow* window, std::int32_t width, std::int32_t height) {
            auto data = mono::window::user_ptr(window);
            data->buffer_width  = width;
            data->buffer_height = height;
        });
        glfwSetWindowPosCallback(m_window,
        [](GLFWwindow* window, std::int32_t xpos, std::int32_t ypos) {
            auto data = mono::window::user_ptr(window);
            data->xpos = xpos;
            data->ypos = ypos;
        });

        glfwGetFramebufferSize(m_window, &m_data.buffer_width, &m_data.buffer_height);
    }
    ~window() {
        glfwTerminate();
    }

    [[nodiscard]] auto shouldclose() const -> bool { return glfwWindowShouldClose(m_window); }
    [[nodiscard]] auto width()  const -> std::int32_t { return m_data.width;  }
    [[nodiscard]] auto height() const -> std::int32_t { return m_data.height; }
    [[nodiscard]] auto buffer_width()  const -> std::int32_t { return m_data.buffer_width; }
    [[nodiscard]] auto buffer_height() const -> std::int32_t { return m_data.buffer_height; }
    [[nodiscard]] auto x() const -> std::int32_t { return m_data.xpos; }
    [[nodiscard]] auto y() const -> std::int32_t { return m_data.ypos; }
    auto set_position(std::int32_t const& x, std::int32_t const& y) -> void {
        glfwSetWindowPos(m_window, x, y);
    }
    auto swap() -> void { glfwSwapBuffers(m_window); }
    static auto poll() -> void { glfwPollEvents(); }

    auto get_key(std::int32_t key) -> std::int32_t {
        return glfwGetKey(m_window, key);
    }

  public:
    [[nodiscard]] auto str() const -> std::string {
        std::string str{"mono::window { "};
        str += "title: \"" + m_data.title + "\", ";
        str += "width: "  + std::to_string(m_data.width)  + ", ";
        str += "height: " + std::to_string(m_data.height) + ", ";
        str += "buffer_width: "  + std::to_string(m_data.buffer_width) + ", ";
        str += "buffer_height: " + std::to_string(m_data.buffer_height) + " }";
        return str;
    }

  private:
    GLFWwindow* m_window{nullptr};

    struct data {
        std::string title;
        std::int32_t width;
        std::int32_t height;
        std::int32_t buffer_width;
        std::int32_t buffer_height;
        std::int32_t xpos;
        std::int32_t ypos;
    };
    data m_data{};

  private:
    static auto user_ptr(GLFWwindow* window) -> window::data* {
        return static_cast<window::data*>(glfwGetWindowUserPointer(window));
    }
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
uniform vec2  u_resolution;
uniform vec2  u_mouse;
uniform vec4  u_color;

uniform sampler2D u_texture;

void main() {
    o_color = io_color;
}
)";

class shader {
  public:
    shader(std::string const& vertex_source, std::string const& fragment_source) {
        auto vs = shader::compile(GL_VERTEX_SHADER,   vertex_source.c_str());
        auto fs = shader::compile(GL_FRAGMENT_SHADER, fragment_source.c_str());
        m_id    = shader::link(vs, fs);
    }
    ~shader() {
        glDeleteProgram(m_id);
    }

    auto bind() const -> void { glUseProgram(m_id); }
    auto unbind() const -> void { glUseProgram(0); }

  public:
    auto str() const -> std::string {
        std::string str{"mono::shader { "};
        str += "id: " + std::to_string(m_id);
        str += " }";
        return str;
    }

  private:
    static auto compile(std::uint32_t const& type, char const* source) -> std::uint32_t {
        std::uint32_t shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        std::int32_t is_success;
        constexpr auto LOG_SIZE = 512;
        static char info_log[LOG_SIZE];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_success);
        if (!is_success) {
            glGetShaderInfoLog(shader, LOG_SIZE, nullptr, info_log);
            spdlog::error("SHADER::{}SHADER::COMPILE_FAILED {}",
                    type == GL_VERTEX_SHADER ? "VERTEX_" : "FRAGMENT_",
                    info_log);
            throw std::runtime_error("Shader compilation error");
        }

        return shader;
    }
    static auto link(std::uint32_t const& fs, std::uint32_t const& vs) -> std::uint32_t {
        std::uint32_t program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        std::int32_t is_success;
        constexpr auto LOG_SIZE = 512;
        static char info_log[LOG_SIZE];
        glGetProgramiv(program, GL_LINK_STATUS, &is_success);
        if (!is_success) {
            glGetProgramInfoLog(program, LOG_SIZE, nullptr, info_log);
            spdlog::error("SHADER::LINK {}", info_log);
            throw std::runtime_error("Shader linking error");
        }

        glUseProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return program;
    }

  private:
    std::uint32_t m_id;
};
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
    std::uint32_t array_buffer;
    std::uint32_t vertex_buffer;
    std::uint32_t index_buffer;
    glGenVertexArrays(1, &array_buffer);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);

    glBindVertexArray(array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBindVertexArray(array_buffer);

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(std::uint32_t), GL_UNSIGNED_INT, nullptr);

        window.swap();
        mono::window::poll();
    }

    glDeleteVertexArrays(1, &array_buffer);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);

    return 0;
}

