/**
 * @file   graphics_context.cpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  OpenGL graphics implementation
 * @date   2022-06-02
 *
 * @copyright Copyright (c) 2022
 */
#include "graphics_context.hpp"
#include "glad/glad.h"

namespace mno {

auto graphics_context::draw_triangles(ref<mno::array_buffer> const& buffer) -> void {
    draw_triangles(*buffer);
}
auto graphics_context::draw_triangles(mno::array_buffer const& buffer) -> void {
    buffer.bind();
    buffer.vertex_buffer()->bind();
    buffer.index_buffer()->bind();
    glDrawElements(GL_TRIANGLES, buffer.index_buffer()->count(), buffer.index_buffer()->type(), nullptr);
}

}  // namespace mno

