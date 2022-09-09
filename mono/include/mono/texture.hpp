/**
 * @file   texture.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Texture implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_TEXTURE_HPP
#define MONO_TEXTURE_HPP

#include <cstdint>

#include "common.hpp"
#include "image.hpp"

namespace mno {
enum class texture_flag : std::uint64_t {
    mipmap      = MONO_BIT(0),
    min_nearest = MONO_BIT(1),
    mag_nearest = MONO_BIT(2),
};

class texture {
  public:
    explicit texture(mno::image const& image);
    texture(std::int32_t const& width, std::int32_t const& height);
    ~texture();

    auto bind(std::uint32_t const& id = 0) const -> void;
    auto unbind() const -> void;

    auto set_image(mno::image const& image) -> void;
    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    [[nodiscard]] auto buffer() const -> std::uint32_t { return m_buffer; }
    auto width()  const -> std::int32_t { return m_width; }
    auto height() const -> std::int32_t { return m_height; }

  private:
    std::uint32_t m_buffer{};
    std::int32_t  m_width;
    std::int32_t  m_height;
};
}  // namespace mno

#endif // MONO_TEXTURE_HPP
