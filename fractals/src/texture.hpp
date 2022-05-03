/**
 * @file   texture.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Texture implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_TEXTURE_HPP
#define FRACTALS_TEXTURE_HPP

#include <cstdint>

namespace mono {
class texture {
  public:
    texture(std::int32_t const& width, std::int32_t const& height);
    ~texture();

    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    auto bind(std::uint32_t const& id = 0) const -> void;
    auto unbind() const -> void;
    auto buffer() const -> std::uint32_t { return m_buffer; }
    auto param(std::uint32_t const& name, std::int32_t const& value) const -> void;

  private:
    std::uint32_t m_buffer{};
    std::int32_t  m_width;
    std::int32_t  m_height;
};
}

#endif //FRACTALS_TEXTURE_HPP
