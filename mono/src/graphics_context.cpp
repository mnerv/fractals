#include "graphics_context.hpp"
#include "glad/glad.h"

namespace mno {

auto graphics_context::draw(render_mode const&, ref<mno::array_buffer> const&) -> void {
}
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

