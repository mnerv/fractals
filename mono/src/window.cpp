/**
 * @file   window.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Window implementation acts as a wrapper for GLFW window.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include <algorithm>
#include "window.hpp"

#include "spdlog/spdlog.h"

namespace mno {
static auto setup_opengl() -> void {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

window::window(const window_props &props) {
    if (!glfwInit()) throw std::runtime_error("Error initializing GLFW!");
    mno::setup_opengl();
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
    // TODO: Better event systems, right now this calls the event handler for each event.
    glfwSetWindowSizeCallback(m_window,
    [](GLFWwindow* window, std::int32_t width, std::int32_t height) {
        auto data = mno::window::user_ptr(window);
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
        auto data = mno::window::user_ptr(window);
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
    //glfwSetWindowContentScaleCallback(m_window,
    //[](GLFWwindow* window, float xscale, float yscale){
    //    auto data = mno::window::user_ptr(window);
    //});
    //glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, std::int32_t iconified) {
    //    auto data = mno::window::user_ptr(window);
    //});
    //glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, std::int32_t maximized) {
    //    auto data = mno::window::user_ptr(window);
    //})
    //glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
    //    auto data = mno::window::user_ptr(window);
    //});
    glfwSetWindowPosCallback(m_window,
    [](GLFWwindow* window, std::int32_t xpos, std::int32_t ypos) {
        auto data = mno::window::user_ptr(window);
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
    [](GLFWwindow* window, mno::f64 xpos, mno::f64 ypos) {
        auto data = mno::window::user_ptr(window);

        auto it = data->events.find(event_type::mouse_move);
        if (it == data->events.end()) return;
        auto fns = it->second;
        auto event = mouse_move_event(xpos, ypos);
        std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
            fn.second(event);
        });
    });
    glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, std::int32_t entered) {
       auto data = mno::window::user_ptr(window);
        mno::f64 pos_x, pos_y;
        glfwGetCursorPos(window, &pos_x, &pos_y);

        if (entered) {
            auto it = data->events.find(event_type::mouse_enter);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = mouse_enter_event(pos_x, pos_y);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        } else {
            auto it = data->events.find(event_type::mouse_leave);
            if (it == data->events.end()) return;
            auto fns = it->second;

            auto event = mouse_leave_event(pos_x, pos_y);
            std::for_each(std::begin(fns), std::end(fns), [&](auto const& fn) {
                fn.second(event);
            });
        }
    });
    glfwSetMouseButtonCallback(m_window,
    [](GLFWwindow* window, std::int32_t button, std::int32_t action, std::int32_t mods) {
        auto data = mno::window::user_ptr(window);
        mno::f64 pos_x, pos_y;
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
    [](GLFWwindow* window, f64 xoffset, f64 yoffset){
        auto data = mno::window::user_ptr(window);
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
        auto data = mno::window::user_ptr(window);
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
        auto data = mno::window::user_ptr(window);
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
        auto data = mno::window::user_ptr(window);
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

auto window::mouse_pos(f64 &x, f64 &y) const -> void {
    glfwGetCursorPos(m_window, &x, &y);
}
auto window::keystate(mno::key const& key) const -> mno::keystate {
    using T = std::underlying_type_t<mno::key>;
    auto const current_state = glfwGetKey(m_window, static_cast<T>(key));
    return static_cast<mno::keystate>(current_state);
}
}  // namespace mno
