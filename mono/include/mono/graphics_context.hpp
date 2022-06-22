/**
 * @file   graphics_context.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Renderer abstraction
 * @date   2022-06-02
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_RENDERER_HPP
#define MONO_RENDERER_HPP

#include <cstdint>

#include "common.hpp"
#include "buffer.hpp"

namespace mno {

enum class render_mode : std::uint32_t {
    triangles = MONO_BIT(0),
    lines     = MONO_BIT(1),
};

class graphics_context {
  public:
    auto draw_triangles(ref<mno::array_buffer> const& buffer) -> void;
    auto draw_triangles(mno::array_buffer const& buffer) -> void;
};

}  // namespace mno

#endif // MONO_RENDERER_HPP
