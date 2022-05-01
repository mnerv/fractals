#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace mono {
auto opengl_version() -> void {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

struct window_properties {
    std::string title   = "no title";
    std::int32_t width  = 738;
    std::int32_t height = 480;
    std::int32_t xpos{INT32_MIN};
    std::int32_t ypos{INT32_MIN};
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
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    mono::window window{};

    std::uint8_t* image_buffer = nullptr;
    image_buffer = new std::uint8_t[std::size_t(window.width()) * std::size_t(window.height()) * 4];
    for (auto i = 0; i < window.height(); i++) {
        for (auto j = 0; j < window.width(); j++) {
            auto index = (i * 4) * window.width() + (j * 4);

            image_buffer[index + 0] = 0;
            image_buffer[index + 1] = 255;
            image_buffer[index + 2] = 0;
            image_buffer[index + 3] = 255;
        }
    }

    std::uint32_t texture_buffer;
    glGenTextures(1, &texture_buffer);
    glBindTexture(GL_TEXTURE_2D, texture_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width(), window.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    spdlog::info(window.str());

    auto is_running = true;
    while (is_running) {
        is_running = !window.shouldclose();
        glViewport(0, 0, window.width(), window.height());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.swap();
        mono::window::poll();
    }
    glDeleteTextures(1, &texture_buffer);
    delete [] image_buffer;
    return 0;
}

