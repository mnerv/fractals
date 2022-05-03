/**
 * @file   buffer.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Buffer abstraction layer for OpenGL.
 *         Vertex, Index and Array buffer interface.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_BUFFER_HPP
#define FRACTALS_BUFFER_HPP

#include <cstdint>
#include "mono.hpp"
#include "texture.hpp"

namespace mono {

class vertex_buffer {
  public:
    vertex_buffer(void const* data, std::uint32_t const& size);
    ~vertex_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class index_buffer {
  public:
    index_buffer(void const* data, std::uint32_t const& size);
    ~index_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class array_buffer {
  public:
    array_buffer();
    ~array_buffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer;
};

class renderbuffer {
  public:
    renderbuffer(std::int32_t const& width, std::int32_t const& height);
    ~renderbuffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    auto buffer() const -> std::uint32_t { return m_buffer; }

  private:
    std::uint32_t m_buffer{};
    std::int32_t  m_width;
    std::int32_t  m_height;
};

class framebuffer {
  public:
    framebuffer();
    ~framebuffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;

    auto texture(texture const& tex) -> void;
    auto render(renderbuffer const& render) -> void;

  private:
    std::uint32_t m_buffer{};
};

}

#endif //FRACTALS_BUFFER_HPP
