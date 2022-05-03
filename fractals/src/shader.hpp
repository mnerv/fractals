/**
 * @file   shader.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Shader implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_SHADER_HPP
#define FRACTALS_SHADER_HPP

#include <cstdint>
#include <string>

#include "mono.hpp"

#include "spdlog/spdlog.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

namespace mono {
class shader {
  public:
    static auto make(std::string const& vertex_source, std::string const& fragment_source) -> local<shader>;

  public:
    shader(std::string const& vertex_source, std::string const& fragment_source);
    ~shader();

    auto bind() const -> void;
    auto unbind() const -> void;

    // uniform uploads
  public:
    auto num(std::string const& name, std::uint32_t const& value) -> void;
    auto num(std::string const& name, std::int32_t const& value) -> void;
    auto num(std::string const& name, float const& value) -> void;
    auto num(std::string const& name, std::int32_t const& count, float const* value) -> void;

    auto vec2(std::string const& name, glm::vec2 const& value) -> void;
    auto vec3(std::string const& name, glm::vec3 const& value) -> void;
    auto vec4(std::string const& name, glm::vec3 const& value) -> void;

    auto mat2(std::string const& name, glm::mat2 const& value, bool const& transpose = false) -> void;
    auto mat3(std::string const& name, glm::mat3 const& value, bool const& transpose = false) -> void;
    auto mat4(std::string const& name, glm::mat4 const& value, bool const& transpose = false) -> void;

  public:
    [[nodiscard]] auto str() const -> std::string;

  private:
    static auto compile(std::uint32_t const& type, char const* source) -> std::uint32_t;
    static auto link(std::uint32_t const& fs, std::uint32_t const& vs) -> std::uint32_t;
    [[nodiscard]] auto uniform_location(std::string const& name) const -> std::int32_t;

  private:
    std::uint32_t m_id;
};
}

#endif //FRACTALS_SHADER_HPP
