/**
 * @file   image.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Basic image implementation.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_IMAGE_HPP
#define FRACTALS_IMAGE_HPP

#include "mono.hpp"
#include <cstdint>

namespace mono {
class image {
  public:
    image(std::int32_t const& width, std::int32_t const& height, std::int32_t const& channels = 4);
    ~image();

    auto resize(std::int32_t const& width, std::int32_t const& height, std::int32_t const& channels = 4) -> void;

    auto width() const -> std::int32_t { return m_width; }
    auto height() const -> std::int32_t { return m_height; }
    auto channels() const -> std::int32_t { return m_channels; }
    auto get(std::int32_t const& x, std::int32_t const& y) const -> std::uint32_t;
    auto set(std::int32_t const& x, std::int32_t const& y, std::uint32_t const& color, std::uint8_t const& alpha = 255) -> void;
    auto set(std::int32_t const& x, std::int32_t const& y,
             std::uint8_t const& red,  std::uint8_t const& green,
             std::uint8_t const& blue, std::uint8_t const& alpha = 255) -> void;

    auto buffer() const -> std::uint8_t* { return m_buffer; }
  private:
    std::uint8_t* m_buffer;

    std::int32_t m_width;
    std::int32_t m_height;
    std::int32_t m_channels;
};
}  // namespace mono

#endif // FRACTALS_IMAGE_HPP
