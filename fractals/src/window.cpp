/**
 * @file   window.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Window abstraction layer for GLFW.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "window.hpp"

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

window::~window() {
    glfwTerminate();
}

auto window::swap() -> void { glfwSwapBuffers(m_window); }
auto window::poll() -> void {
    std::for_each(std::begin(m_keys), std::end(m_keys), [&](auto& key){
        key->update(glfwGetKey(m_window, key->value));
    });
    glfwPollEvents();
}
}
