/**
 * @file   framebuffer.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Frambuffer
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_FRAMEBUFFER_HPP
#define MONO_FRAMEBUFFER_HPP

#include <cstdint>
#include "common.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace mno {
class framebuffer {
  public:
    framebuffer(std::int32_t const& width, std::int32_t const& height);
    framebuffer(ref<mno::texture> const& texture, ref<mno::renderbuffer> const& render);
    ~framebuffer() noexcept;

    auto bind() const -> void;
    auto unbind() const -> void;
    auto width()  const -> std::int32_t { return m_texture->width(); };
    auto height() const -> std::int32_t { return m_texture->height(); };

    auto resize(std::int32_t const& width, std::int32_t const& height) -> void;
    auto texture() -> ref<mno::texture> { return m_texture; }

  private:
    std::uint32_t           m_buffer{};
    ref<mno::texture>      m_texture{nullptr};
    ref<mno::renderbuffer> m_render{nullptr};
};
}  // namespace mno

#endif // MONO_FRAMEBUFFER_HPP
