/**
 * @file   window.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Window abstraction layer for GLFW.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_WINDOW_HPP
#define MONO_WINDOW_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <functional>

#include "common.hpp"
#include "event.hpp"
#include "keyboard.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

namespace mno {
struct window_props {
    std::string  title  = "mno::window";
    std::int32_t width  = 738;
    std::int32_t height = 480;
    std::int32_t xpos{INT32_MIN};
    std::int32_t ypos{INT32_MIN};
};

class window {
  public:
    using event_fn  = std::function<void(event const&)>;
    using event_map = std::unordered_map<std::size_t, event_fn>;

  public:
    explicit window(window_props const& props = {});
    ~window();

    [[nodiscard]] auto shouldclose() const -> bool { return glfwWindowShouldClose(m_window); }
    [[nodiscard]] auto width()  const -> std::int32_t { return m_data.width;  }
    [[nodiscard]] auto height() const -> std::int32_t { return m_data.height; }
    [[nodiscard]] auto buffer_width()  const -> std::int32_t { return m_data.buffer_width; }
    [[nodiscard]] auto buffer_height() const -> std::int32_t { return m_data.buffer_height; }
    [[nodiscard]] auto xpos() const -> std::int32_t { return m_data.xpos; }
    [[nodiscard]] auto ypos() const -> std::int32_t { return m_data.ypos; }
    auto set_position(std::int32_t const& x, std::int32_t const& y) -> void {
        glfwSetWindowPos(m_window, x, y);
    }
    auto swap() -> void;
    auto poll() -> void;
    auto time() const -> double { return glfwGetTime(); }

    auto mouse_pos(f64& x, f64& y) const -> void;
    auto keystate(mno::key const& key) const -> mno::keystate;

    // TODO: Add type constraints and maybe if possible have it so the lambda can have the inherit parameter type
    template <typename Callable>
    auto add_event_listener(event_type const& type, Callable const& func) -> void {
        auto const id = (std::size_t)&func;
        if (m_data.events.find(type) == m_data.events.end()) {
            std::unordered_map<std::size_t, event_fn> fns{{id, func}};
            m_data.events.insert({type, fns});
        } else {
            m_data.events[type].insert({id, func});
        }
    }
    template <typename Callable>
    auto remove_event_listener(event_type const& type, [[maybe_unused]]Callable const& func) -> void {
        auto id  = (std::size_t)&func;
        auto fns = m_data.events.find(type);
        if (fns != std::end(m_data.events))
            fns->second.erase(id);
    }

  public:
    [[nodiscard]] auto str() const -> std::string {
        std::string str{"mno::window { "};
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
        std::string  title;
        std::int32_t width;
        std::int32_t height;
        std::int32_t buffer_width;
        std::int32_t buffer_height;
        std::int32_t xpos;
        std::int32_t ypos;

        std::unordered_map<event_type, event_map> events;
    };
    data m_data{};

  private:
    static auto user_ptr(GLFWwindow* window) -> window::data* {
        return static_cast<window::data*>(glfwGetWindowUserPointer(window));
    }
};
}  // namespace mno

#endif // MONO_WINDOW_HPP
