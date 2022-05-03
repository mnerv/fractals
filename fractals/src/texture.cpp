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

#include "spdlog/spdlog.h"

namespace mono {

texture::texture(std::int32_t const& width, std::int32_t const& height) {
    glGenTextures(1, &m_buffer);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}
texture::texture(mono::image const& image) {
    glGenTextures(1, &m_buffer);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.buffer());
}
texture::~texture() {
    glDeleteTextures(1, &m_buffer);
}

auto texture::resize(std::int32_t const& width, std::int32_t const& height) -> void {
    glBindTexture(GL_TEXTURE_2D, m_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}
auto texture::bind(std::uint32_t const& id) const -> void {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, m_buffer);
}
auto texture::mipmap() -> void {
    glGenerateMipmap(GL_TEXTURE_2D);
}
auto texture::unbind() const -> void { glBindTexture(GL_TEXTURE_2D, 0); }
auto texture::param(std::uint32_t const& name, std::int32_t const& value) const -> void {
    glTexParameteri(GL_TEXTURE_2D, name, value);
}

}

