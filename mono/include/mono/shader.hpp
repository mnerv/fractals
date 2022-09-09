/**
 * @file   shader.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Shader implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_SHADER_HPP
#define MONO_SHADER_HPP

#include <cstdint>
#include <string>

#include "common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

namespace mno {
class shader {
  public:
    static auto make(std::string const& vertex_source, std::string const& fragment_source) -> local<shader>;
    static auto make() -> local<shader>;

    // shader types
    // https://www.khronos.org/opengl/wiki/OpenGL_Type
    enum class type : std::uint32_t {
        none = 0,
        // primitive types
        b8,  // bool
        i8,  u8,
        i16, u16,
        i32, u32, p32,
        f16, f32, f64,

        // vector types (f32, i32, f64)
        vec2,  vec3,  vec4,
        ivec2, ivec3, ivec4,
        dvec2, dvec3, dvec4,

        // matrx nxn types (f32)
        mat2, mat3, mat4,
    };

  public:
    shader(std::string const& vertex_source, std::string const& fragment_source);
    ~shader();

    auto bind() const -> void;
    auto unbind() const -> void;

    // uniform uploads
  public:
    auto num(std::string const& name, mno::u32 const& value) -> void;
    auto num(std::string const& name, mno::i32 const& value) -> void;
    auto num(std::string const& name, mno::f32 const& value) -> void;
    auto num(std::string const& name, mno::i32 const& count, float const* value) -> void;

    auto vec2(std::string const& name, glm::vec2 const& value) -> void;
    auto vec3(std::string const& name, glm::vec3 const& value) -> void;
    auto vec4(std::string const& name, glm::vec3 const& value) -> void;

    auto mat2(std::string const& name, glm::mat2 const& value, bool const& transpose = false) -> void;
    auto mat3(std::string const& name, glm::mat3 const& value, bool const& transpose = false) -> void;
    auto mat4(std::string const& name, glm::mat4 const& value, bool const& transpose = false) -> void;

  public:
    [[nodiscard]] auto str() const -> std::string;

  private:
    static auto compile(mno::u32 const& type, char const* source) -> mno::u32;
    static auto link(mno::u32 const& fs, mno::u32 const& vs) -> mno::u32;
    [[nodiscard]] auto uniform_location(std::string const& name) const -> mno::i32;

  private:
    mno::u32 m_id;
};
}  // namespace mno

#endif // MONO_SHADER_HPP
