/**
 * @file   texture.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Texture implemenation for OpenGL.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include "texture.hpp"
#include "glad/glad.h"

namespace mono {

texture::texture(std::int32_t const& width, std::int32_t const& height)
    : m_width(width), m_height(height) {
    glGenTextures(1, &m_buffer);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}
texture::texture(mono::image const& image)
    : m_width(image.width()), m_height(image.height()) {
    glGenTextures(1, &m_buffer);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.buffer());
}
texture::~texture() {
    glDeleteTextures(1, &m_buffer);
}

auto texture::resize(std::int32_t const& width, std::int32_t const& height) -> void {
    m_width  = width;
    m_height = height;
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}
auto texture::bind(std::uint32_t const& id) const -> void {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
}
auto texture::unbind() const -> void { glBindTexture(GL_TEXTURE_2D, 0); }

}  // namespace mono

