#ifndef FRACTALS_WINDOW_HPP
#define FRACTALS_WINDOW_HPP

#include <cstdint>
#include <stdexcept>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace mono {
struct window_properties {
    std::string  title  = "no title";
    std::int32_t width  = 738;
    std::int32_t height = 480;
    std::int32_t xpos{INT32_MIN};
    std::int32_t ypos{INT32_MIN};
};

class window {
  public:
    explicit window(window_properties const& props = {});
    ~window();

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
}

#endif //FRACTALS_WINDOW_HPP
