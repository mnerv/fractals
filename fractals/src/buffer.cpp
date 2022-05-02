#include "buffer.hpp"

#include "glad/glad.h"

namespace mono {
vertex_buffer::vertex_buffer(const void *data, std::uint32_t const& size) {
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
vertex_buffer::~vertex_buffer() {
    glDeleteBuffers(1, &m_buffer);
}

auto vertex_buffer::bind() const -> void { glBindBuffer(GL_ARRAY_BUFFER, m_buffer); }
auto vertex_buffer::unbind() const -> void { glBindBuffer(GL_ARRAY_BUFFER, 0); }

index_buffer::index_buffer(void const* data, std::uint32_t const& size) {
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
index_buffer::~index_buffer() { glDeleteBuffers(1, &m_buffer); }

auto index_buffer::bind() const -> void { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer); }
auto index_buffer::unbind() const -> void { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

array_buffer::array_buffer() {
    glGenVertexArrays(1, &m_buffer);
    glBindVertexArray(m_buffer);
}
array_buffer::~array_buffer() {
    glDeleteVertexArrays(1, &m_buffer);
}

auto array_buffer::bind() const -> void { glBindVertexArray(m_buffer); }
auto array_buffer::unbind() const -> void { glBindVertexArray(0); }
}