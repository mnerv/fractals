/**
 * @file   framebuffer.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Frambuffer
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "framebuffer.hpp"

#include "glad/glad.h"

namespace mono {
framebuffer::framebuffer(std::int32_t const& width, std::int32_t const& height)
        : framebuffer(make_ref<mono::texture>(width, height),make_ref<mono::renderbuffer>(width, height)) {}

framebuffer::framebuffer(ref<mono::texture> const& texture, ref<mono::renderbuffer> const& render)
        : m_texture(texture), m_render(render) {
    glGenFramebuffers(1, &m_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
    m_texture->bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture->buffer(), 0);
    m_render->bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render->buffer());
}
framebuffer::~framebuffer() noexcept {
    glDeleteFramebuffers(1, &m_buffer);
}

auto framebuffer::resize(std::int32_t const& width, std::int32_t const& height) -> void {
    this->bind();
    m_texture->resize(width, height);
    m_render->resize(width, height);
}
auto framebuffer::bind() const -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
}
auto framebuffer::unbind() const -> void { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
}

