#include "shader.hpp"

#include "glad/glad.h"

namespace mono {
//static auto basic_vertex_shader = R"(
//)";
//static auto basic_fragment_shader = R"(
//)";

auto shader::make(const std::string &vertex_source, const std::string &fragment_source) -> mono::ref<shader> {
    return make_ref<shader>(vertex_source, fragment_source);
}

shader::shader(const std::string &vertex_source, const std::string &fragment_source) {
    auto vs = shader::compile(GL_VERTEX_SHADER,   vertex_source.c_str());
    auto fs = shader::compile(GL_FRAGMENT_SHADER, fragment_source.c_str());
    m_id    = shader::link(vs, fs);
}
shader::~shader() {
    glDeleteProgram(m_id);
}

auto shader::bind() const -> void { glUseProgram(m_id); }
auto shader::unbind() const -> void { glUseProgram(0); }

auto shader::num(std::string const& name, std::uint32_t const& value) -> void {
    bind();
    glUniform1ui(uniform_location(name), value);
}
auto shader::num(std::string const& name, std::int32_t const& value) -> void {
    bind();
    glUniform1i(uniform_location(name), value);
}
auto shader::num(std::string const& name, float const& value) -> void {
    bind();
    glUniform1f(uniform_location(name), value);
}
auto shader::num(std::string const& name, std::int32_t const& count, float const* value) -> void {
    bind();
    glUniform1fv(uniform_location(name), count, value);
}

auto shader::vec2(std::string const& name, glm::vec2 const& value) -> void {
    bind();
    glUniform2fv(uniform_location(name), 1, glm::value_ptr(value));
}
auto shader::vec3(std::string const& name, glm::vec3 const& value) -> void {
    bind();
    glUniform3fv(uniform_location(name), 1, glm::value_ptr(value));
}
auto shader::vec4(std::string const& name, glm::vec3 const& value) -> void {
    bind();
    glUniform4fv(uniform_location(name), 1, glm::value_ptr(value));
}

auto shader::mat2(std::string const& name, glm::mat2 const& value, bool const& transpose) -> void {
    bind();
    glUniformMatrix2fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}
auto shader::mat3(std::string const& name, glm::mat3 const& value, bool const& transpose) -> void {
    bind();
    glUniformMatrix3fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}
auto shader::mat4(std::string const& name, glm::mat4 const& value, bool const& transpose) -> void {
    bind();
    glUniformMatrix4fv(uniform_location(name), 1, (transpose ? GL_TRUE : GL_FALSE),
                       glm::value_ptr(value));
}

auto shader::compile(std::uint32_t const& type, char const* source) -> std::uint32_t {
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
auto shader::link(std::uint32_t const& fs, std::uint32_t const& vs) -> std::uint32_t {
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
auto shader::uniform_location(std::string const& name) const -> std::int32_t {
    return glGetUniformLocation(m_id, name.c_str());
}
}
