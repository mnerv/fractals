/**
 * @file   framebuffer.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Frambuffer
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_FRAMEBUFFER_HPP
#define FRACTALS_FRAMEBUFFER_HPP

#include <cstdint>
#include "mono.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace mono {
class framebuffer {
  public:
    framebuffer(std::int32_t const& width, std::int32_t const& height);
    framebuffer(ref<mono::texture> const& texture, ref<mono::renderbuffer> const& render);
    ~framebuffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;
    auto width()  const -> std::int32_t { return m_texture->width(); };
    auto height() const -> std::int32_t { return m_texture->height(); };

    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    auto texture() -> ref<mono::texture> { return m_texture; }

  private:
    std::uint32_t           m_buffer{};
    ref<mono::texture>      m_texture{nullptr};
    ref<mono::renderbuffer> m_render{nullptr};
};
}  // namespace mono

#endif // FRACTALS_FRAMEBUFFER_HPP
