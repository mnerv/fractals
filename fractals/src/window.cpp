/**
 * @file   window.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Window abstraction layer for GLFW.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include <algorithm>
#include "window.hpp"

#include "spdlog/spdlog.h"

namespace mono {
static auto setup_opengl() -> void {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

window::window(const window_props &props) {
    if (!glfwInit()) throw std::runtime_error("Error initializing GLFW!");
    mono::setup_opengl();
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

    // Register events
    glfwSetWindowUserPointer(m_window, &m_data);
    glfwSetWindowSizeCallback(m_window,
    [](GLFWwindow* window, std::int32_t width, std::int32_t height) {
        auto data = mono::window::user_ptr(window);
        data->width  = width;
        data->height = height;

        auto it = data->events.find(event_type::window_resize);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = window_resize_event(width, height);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetFramebufferSizeCallback(m_window,
    [](GLFWwindow* window, std::int32_t width, std::int32_t height) {
        auto data = mono::window::user_ptr(window);
        data->buffer_width  = width;
        data->buffer_height = height;

        auto it = data->events.find(event_type::buffer_resize);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = buffer_resize_event(width, height);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetWindowPosCallback(m_window,
    [](GLFWwindow* window, std::int32_t xpos, std::int32_t ypos) {
        auto data = mono::window::user_ptr(window);
        data->xpos = xpos;
        data->ypos = ypos;

        auto it = data->events.find(event_type::window_move);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = window_move_event(xpos, ypos);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetCursorPosCallback(m_window,
    [](GLFWwindow* window, mono::f64 xpos, mono::f64 ypos) {
        auto data = static_cast<window::data*>(glfwGetWindowUserPointer(window));

        auto it = data->events.find(event_type::mouse_move);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = mouse_move_event(xpos, ypos);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetMouseButtonCallback(m_window,
    [](GLFWwindow* window, std::int32_t button, std::int32_t action, std::int32_t mods) {
        auto data = static_cast<window::data*>(glfwGetWindowUserPointer(window));
        double pos_x, pos_y;
        glfwGetCursorPos(window, &pos_x, &pos_y);

        if (action == GLFW_PRESS) {
            auto it = data->events.find(event_type::mouse_press);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = mouse_press_event(button, mods, pos_x, pos_y);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        } else {
            auto it = data->events.find(event_type::mouse_release);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = mouse_release_event(button, mods, pos_x, pos_y);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        }
    });
    glfwSetScrollCallback(m_window,
    [](GLFWwindow* window, double xoffset, double yoffset){
        auto data = static_cast<window::data*>(glfwGetWindowUserPointer(window));

        auto it = data->events.find(event_type::mouse_wheel);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = mouse_wheel_event(xoffset, yoffset);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetKeyCallback(m_window,
    [](GLFWwindow* window, std::int32_t key, std::int32_t code, std::int32_t action, std::int32_t mods) {
        auto data = mono::window::user_ptr(window);
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            auto it = data->events.find(event_type::key_down);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = key_down_event(key, code, mods, action == GLFW_REPEAT);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        } else {
            auto it = data->events.find(event_type::key_up);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = key_up_event(key, code, mods);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        }
    });
    glfwSetCharCallback(m_window,
    [](GLFWwindow* window, unsigned int codepoint) {
        auto data = static_cast<window::data*>(glfwGetWindowUserPointer(window));
        auto it = data->events.find(event_type::key_typed);
        if (it == data->events.end()) return;
        auto fns = it->second;

        auto event = key_typed_event(codepoint);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetDropCallback(m_window,
    [](GLFWwindow* window, std::int32_t count, char const** paths){
        auto data = mono::window::user_ptr(window);
        auto it = data->events.find(event_type::drop);
        if (it == data->events.end()) return;

        std::vector<std::string> vec_paths{paths, paths + count};
        auto fns = it->second;
        auto event = drop_event(vec_paths);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });

    glfwGetFramebufferSize(m_window, &m_data.buffer_width, &m_data.buffer_height);
}

window::~window() {
    glfwTerminate();
}

auto window::swap() -> void { glfwSwapBuffers(m_window); }
auto window::poll() -> void {
    glfwPollEvents();
}

auto window::get_key(mono::key const& key) -> mono::keystate {
    using T = std::underlying_type_t<mono::key>;
    auto const current_state = glfwGetKey(m_window, static_cast<T>(key));
    return static_cast<mono::keystate>(current_state);
}
}
