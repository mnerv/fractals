/**
 * @file   buffer.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Buffer implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "buffer.hpp"
#include <numeric>

#include "glad/glad.h"
#include "spdlog/spdlog.h"

namespace mono {

buffer_layout::buffer_layout(std::initializer_list<buffer_element> const& elements)
    : m_elements(elements) {
    m_stride = compute_stride();
    compute_offset();  // set the correct element data offset
}

auto buffer_layout::compute_stride() const -> std::size_t {
    auto strides = std::reduce(std::begin(m_elements), std::end(m_elements), std::size_t(0),
    [](auto& acc, auto const& b) {
        return acc + buffer_element::shader_type_size(b.type);
    });
    return strides;
}

auto buffer_layout::compute_offset() -> void {
    std::uint32_t offset = 0;
    for (std::size_t i = 0; i < m_elements.size(); i++) {
        m_elements[i].offset = offset;
        offset += buffer_element::shader_type_size(m_elements[i].type);
    }
}

vertex_buffer::vertex_buffer(const void *data, std::uint32_t const& size) {
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
vertex_buffer::~vertex_buffer() noexcept {
    glDeleteBuffers(1, &m_buffer);
}

auto vertex_buffer::bind() const -> void { glBindBuffer(GL_ARRAY_BUFFER, m_buffer); }
auto vertex_buffer::unbind() const -> void { glBindBuffer(GL_ARRAY_BUFFER, 0); }

index_buffer::index_buffer(void const* data, std::uint32_t const& size) {
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
index_buffer::~index_buffer() noexcept { glDeleteBuffers(1, &m_buffer); }

auto index_buffer::bind() const -> void { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer); }
auto index_buffer::unbind() const -> void { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

array_buffer::array_buffer() {
    glGenVertexArrays(1, &m_buffer);
    glBindVertexArray(m_buffer);
}
array_buffer::~array_buffer() noexcept {
    glDeleteVertexArrays(1, &m_buffer);
}

auto array_buffer::bind() const -> void { glBindVertexArray(m_buffer); }
auto array_buffer::unbind() const -> void { glBindVertexArray(0); }

renderbuffer::renderbuffer(std::int32_t const& width, std::int32_t const& height) {
    glGenRenderbuffers(1, &m_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
renderbuffer::~renderbuffer() noexcept {
    glDeleteRenderbuffers(1, &m_buffer);
}

auto renderbuffer::resize(std::int32_t const& width, std::int32_t const& height) const -> void {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    unbind();
}
auto renderbuffer::bind() const -> void { glBindRenderbuffer(GL_RENDERBUFFER, m_buffer); }
auto renderbuffer::unbind() const -> void { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

}

