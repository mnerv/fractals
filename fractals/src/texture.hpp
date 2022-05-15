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

#include "mono.hpp"
#include "image.hpp"

namespace mono {
class texture {
  public:
    explicit texture(mono::image const& image);
    texture(std::int32_t const& width, std::int32_t const& height);
    ~texture();

    auto bind(std::uint32_t const& id = 0) const -> void;
    auto unbind() const -> void;

    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    [[nodiscard]] auto buffer() const -> std::uint32_t { return m_buffer; }
    auto width()  const -> std::int32_t { return m_width; }
    auto height() const -> std::int32_t { return m_height; }

  private:
    std::uint32_t m_buffer{};
    std::int32_t  m_width;
    std::int32_t  m_height;
};
}  // namespace mono

#endif // FRACTALS_TEXTURE_HPP
