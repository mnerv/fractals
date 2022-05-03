/**
 * @file   buffer.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Buffer implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "buffer.hpp"

#include "glad/glad.h"
#include "spdlog/spdlog.h"

namespace mono {
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

auto renderbuffer::resize(std::int32_t const& width, std::int32_t const& height) -> void {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    unbind();
}
auto renderbuffer::bind() const -> void { glBindRenderbuffer(GL_RENDERBUFFER, m_buffer); }
auto renderbuffer::unbind() const -> void { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

framebuffer::framebuffer() {
    glGenFramebuffers(1, &m_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
}
framebuffer::~framebuffer() noexcept {
    glDeleteFramebuffers(1, &m_buffer);
}

auto framebuffer::bind() const -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
}
auto framebuffer::unbind() const -> void { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
auto framebuffer::texture(mono::texture const& tex) -> void {
    tex.bind();
    tex.param(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tex.param(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.buffer(), 0);
}
auto framebuffer::render(renderbuffer const& render) -> void {
    render.bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render.buffer());
}

}

