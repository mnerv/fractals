/**
 * @file   shader.cpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Shader implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "shader.hpp"

#include "spdlog/spdlog.h"
#include "glad/glad.h"

namespace mno {
static auto basic_vertex_shader = R"(#version 410 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;

out vec4 io_color;
out vec2 io_uv;

void main() {
    io_color = a_color;
    io_uv    = a_uv;
    gl_Position = vec4(a_position, 1.0);
}
)";
static auto basic_fragment_shader = R"(#version 410 core
layout(location = 0) out vec4 o_color;

in vec4 io_color;
in vec2 io_uv;
uniform sampler2D u_texture;

void main() {
    o_color = io_color;
}
)";

auto shader::make(std::string const& vertex_source, std::string const& fragment_source) -> local<shader> {
    return make_local<shader>(vertex_source, fragment_source);
}
auto shader::make() -> local<shader> {
    return make_local<shader>(basic_vertex_shader, basic_fragment_shader);
}

shader::shader(std::string const& vertex_source, std::string const& fragment_source) {
    auto vs = shader::compile(GL_VERTEX_SHADER,   vertex_source.c_str());
    auto fs = shader::compile(GL_FRAGMENT_SHADER, fragment_source.c_str());
    m_id    = shader::link(vs, fs);
}
shader::~shader() {
    glDeleteProgram(m_id);
}

auto shader::bind() const -> void { glUseProgram(m_id); }
auto shader::unbind() const -> void { glUseProgram(0); }

auto shader::num(std::string const& name, mno::u32 const& value) -> void {
    glUniform1ui(uniform_location(name), value);
}
auto shader::num(std::string const& name, mno::i32 const& value) -> void {
    glUniform1i(uniform_location(name), value);
}
auto shader::num(std::string const& name, mno::f32 const& value) -> void {
    glUniform1f(uniform_location(name), value);
}
auto shader::num(std::string const& name, mno::i32 const& count, float const* value) -> void {
    glUniform1fv(uniform_location(name), count, value);
}

auto shader::vec2(std::string const& name, glm::vec2 const& value) -> void {
    glUniform2fv(uniform_location(name), 1, glm::value_ptr(value));
}
auto shader::vec3(std::string const& name, glm::vec3 const& value) -> void {
    glUniform3fv(uniform_location(name), 1, glm::value_ptr(value));
}
auto shader::vec4(std::string const& name, glm::vec3 const& value) -> void {
    glUniform4fv(uniform_location(name), 1, glm::value_ptr(value));
}

auto shader::mat2(std::string const& name, glm::mat2 const& value, bool const& transpose) -> void {
    glUniformMatrix2fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}
auto shader::mat3(std::string const& name, glm::mat3 const& value, bool const& transpose) -> void {
    glUniformMatrix3fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}
auto shader::mat4(std::string const& name, glm::mat4 const& value, bool const& transpose) -> void {
    glUniformMatrix4fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}

auto shader::str() const -> std::string {
    std::string str{"mno::shader { "};
    str += "id: " + std::to_string(m_id);
    str += " }";
    return str;
}

auto shader::compile(mno::u32 const& type, char const* source) -> mno::u32 {
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
auto shader::link(mno::u32 const& fs, mno::u32 const& vs) -> mno::u32 {
    mno::u32 program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    mno::i32 is_success;
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
auto shader::uniform_location(std::string const& name) const -> mno::i32 {
    return glGetUniformLocation(m_id, name.c_str());
}
}  // namespace mno
