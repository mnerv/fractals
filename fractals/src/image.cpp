/**
 * @file   image.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Basic image implementation.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#include <iostream>
#include "image.hpp"

namespace mono {
image::image(std::int32_t const& width, std::int32_t const& height, std::int32_t const& channels)
    : m_width(width), m_height(height), m_channels(channels) {
    m_buffer = new std::uint8_t[std::size_t(m_width * m_height * m_channels)];
}
image::~image() {
    delete[] m_buffer;
}

auto image::resize(std::int32_t const& width, std::int32_t const& height,
                   std::int32_t const& channels) -> void {
    if (m_width == width && m_height == height && m_channels == channels) return;
    m_width    = width;
    m_height   = height;
    m_channels = channels;
    delete[] m_buffer;
    m_buffer = new std::uint8_t[std::size_t(m_width * m_height * m_channels)];
}

auto image::set(std::int32_t const& x, std::int32_t const& y, std::uint32_t const& color, std::uint8_t const& alpha) -> void {
    auto index = (y * m_channels) * m_width + (x * m_channels);
    m_buffer[index + 0] = (color >> 16) & 0xFF;
    m_buffer[index + 1] = (color >>  8) & 0xFF;
    m_buffer[index + 2] = (color >>  0) & 0xFF;
    m_buffer[index + 3] = alpha;
}
auto image::set(std::int32_t const& x, std::int32_t const& y,
                std::uint8_t const& red,  std::uint8_t const& green,
                std::uint8_t const& blue, std::uint8_t const& alpha) -> void {
    auto index = y * m_channels * m_width + (x * m_channels);
    m_buffer[index + 0] = red;
    m_buffer[index + 1] = green;
    m_buffer[index + 2] = blue;
    m_buffer[index + 3] = alpha;
}
}  // namespace mono

