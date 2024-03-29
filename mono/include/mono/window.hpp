/**
 * @file   window.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
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
#include <concepts>

#include "common.hpp"
#include "event.hpp"
#include "keyboard.hpp"
#include "graphics_context.hpp"

#define GLFW_INCLUDE_NONE
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

template <typename T>
concept EventFunc = std::is_invocable_r_v<void, T, event const&>;

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
    [[nodiscard]] auto xscale() const -> mno::f32 { return m_data.xscale; }
    [[nodiscard]] auto yscale() const -> mno::f32 { return m_data.yscale; }
    [[nodiscard]] auto graphics_context() -> ref<mno::graphics_context> { return m_graphics_context; };

    auto set_position(std::int32_t const& x, std::int32_t const& y) -> void;
    auto set_window_size(std::int32_t const& width, std::int32_t const& height) -> void;

    auto window_pos(std::int32_t& x, std::int32_t& y)  const -> void;
    auto window_size(std::int32_t& width, std::int32_t& height) const -> void;
    auto buffer_size(std::int32_t& width, std::int32_t& height) const -> void;
    auto content_scale(mno::f32& x, mno::f32& y) const -> void;

    auto swap() -> void;
    auto poll() -> void;
    [[nodiscard]] auto time() const -> mno::f64;

    auto mouse_pos(mno::f64& x, mno::f64& y) const -> void;
    [[nodiscard]] auto keystate(mno::key const& key) const -> mno::keystate;

    auto add_event_listener(event_type const& type, EventFunc auto const& func) -> void {
        auto const id = std::size_t(&func);
        if (m_data.events.find(type) == m_data.events.end()) {
            std::unordered_map<std::size_t, event_fn> fns{{id, func}};
            m_data.events.insert({type, fns});
        } else {
            m_data.events[type].insert({id, func});
        }
    }
    auto remove_event_listener(event_type const& type, EventFunc auto const& func) -> void {
        auto const id = std::size_t(&func);
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
    GLFWwindow*                m_window{nullptr};
    ref<mno::graphics_context> m_graphics_context{nullptr};

    struct data {
        std::string  title;
        std::int32_t width;
        std::int32_t height;
        std::int32_t buffer_width;
        std::int32_t buffer_height;
        std::int32_t xpos;
        std::int32_t ypos;
        mno::f32     xscale;
        mno::f32     yscale;

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
