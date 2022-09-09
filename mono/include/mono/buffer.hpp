/**
 * @file   buffer.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Buffer abstraction layer for OpenGL.
 *         Vertex, Index and Array buffer interface.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_BUFFER_HPP
#define MONO_BUFFER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "common.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace mno {

struct buffer_element {
    shader::type  type;
    std::string   name;
    bool          normalised = false;
    std::uint32_t offset     = 0;

    // shader data types size in bytes
    // return size in bytes
    inline static auto shader_type_size(shader::type const& type) -> std::uint32_t {
        switch(type) {
            case shader::type::i8:
            case shader::type::u8:    return 1;

            case shader::type::i16:
            case shader::type::u16:
            case shader::type::f16:   return 2;

            case shader::type::i32:
            case shader::type::u32:
            case shader::type::p32:
            case shader::type::f32:   return 4;
            case shader::type::f64:   return 8;

            case shader::type::ivec2:
            case shader::type::vec2:  return 4 * 2;

            case shader::type::ivec3:
            case shader::type::vec3:  return 4 * 3;

            case shader::type::ivec4:
            case shader::type::vec4:  return 4 * 4;

            case shader::type::dvec2: return 8 * 2;
            case shader::type::dvec3: return 8 * 3;
            case shader::type::dvec4: return 8 * 4;

            case shader::type::mat2:  return 4 * 2 * 2;
            case shader::type::mat3:  return 4 * 3 * 3;
            case shader::type::mat4:  return 4 * 4 * 4;

            default: return 0;
        }
    }

    inline static auto component_count(shader::type const& type) -> std::int32_t {
        switch(type) {
            case shader::type::vec2:
            case shader::type::ivec2:
            case shader::type::dvec2: return 2;

            case shader::type::vec3:
            case shader::type::ivec3:
            case shader::type::dvec3: return 3;

            case shader::type::vec4:
            case shader::type::ivec4:
            case shader::type::dvec4: return 4;

            case shader::type::mat2:  return 2 * 2;
            case shader::type::mat3:  return 3 * 3;
            case shader::type::mat4:  return 4 * 4;

            default: return 1;
        }
    }
};

class buffer_layout {
  public:
    buffer_layout(std::initializer_list<buffer_element> const& elements);
    ~buffer_layout() = default;

    auto stride() const -> std::size_t { return m_stride; };
    auto buffer_elements() const -> std::vector<buffer_element> const& { return m_elements; }

    auto begin() -> std::vector<buffer_element>::iterator { return std::begin(m_elements); }
    auto end()   -> std::vector<buffer_element>::iterator { return std::end(m_elements); }
    auto begin() const -> std::vector<buffer_element>::const_iterator { return std::begin(m_elements); }
    auto end()   const -> std::vector<buffer_element>::const_iterator { return std::end(m_elements); }

  private:
    auto compute_stride() const -> std::size_t;
    auto compute_offset() -> void;

  private:
    std::size_t                 m_stride;
    std::vector<buffer_element> m_elements;
};

class vertex_buffer {
  public:
    vertex_buffer(void const* data, std::uint32_t const& size, buffer_layout const& layout);
    ~vertex_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto set_layout(buffer_layout const& layout) -> void { m_layout = layout; }
    auto layout() const -> buffer_layout const& { return m_layout; }

  public:
    static auto make(void const* data, std::uint32_t const& size, buffer_layout const& layout) -> local<vertex_buffer>;

  private:
    std::uint32_t m_buffer{};
    buffer_layout m_layout{};
};

class index_buffer {
  public:
    index_buffer(void const* data, std::uint32_t const& size, std::int32_t const& count);
    ~index_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto count() const -> std::int32_t { return m_count; }
    auto type() const -> std::uint32_t { return m_type; }

  public:
    static auto make(void const* data, std::uint32_t const& size, std::int32_t const& count) -> local<index_buffer>;

  private:
    std::uint32_t m_buffer{};
    std::int32_t  m_count;
    std::uint32_t  m_type;
};

class renderbuffer {
  public:
    renderbuffer(std::int32_t const& width, std::int32_t const& height);
    ~renderbuffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto resize(std::int32_t const& width, std::int32_t const& height) const -> void;
    [[nodiscard]] auto buffer() const -> std::uint32_t { return m_buffer; }

  private:
    std::uint32_t m_buffer{};
};

class array_buffer {
  public:
    array_buffer();
    ~array_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto add_vertex_buffer(ref<mno::vertex_buffer> const& vertex_buffer) -> void;
    auto set_index_buffer(ref<mno::index_buffer> const& index_buffer) -> void { m_index_buffer = index_buffer; }
    auto vertex_buffer() const -> ref<mno::vertex_buffer> { return m_vertex_buffer; }
    auto index_buffer() const -> ref<mno::index_buffer> { return m_index_buffer; }

  private:
    std::uint32_t m_buffer{};
    ref<mno::vertex_buffer>       m_vertex_buffer;
    ref<mno::index_buffer>  m_index_buffer;
};

}  // namespace mno

#endif // MONO_BUFFER_HPP
