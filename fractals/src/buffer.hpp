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

namespace mono {

class vertex_buffer {
  public:
    vertex_buffer(void const* data, std::uint32_t const& size);
    ~vertex_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class index_buffer {
  public:
    index_buffer(void const* data, std::uint32_t const& size);
    ~index_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class array_buffer {
  public:
    array_buffer();
    ~array_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer;
};

}

#endif //FRACTALS_BUFFER_HPP
