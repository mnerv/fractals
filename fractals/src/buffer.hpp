#ifndef FRACTALS_BUFFER_HPP
#define FRACTALS_BUFFER_HPP

#include <cstdint>

namespace mono {

class vertex_buffer {
  public:
    vertex_buffer(void const* data, std::uint32_t const& size);
    ~vertex_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class index_buffer {
  public:
    index_buffer(void const* data, std::uint32_t const& size);
    ~index_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer{};
};

class array_buffer {
  public:
    array_buffer();
    ~array_buffer();

    auto bind() const -> void;
    auto unbind() const -> void;

  private:
    std::uint32_t m_buffer;
};

}

#endif //FRACTALS_BUFFER_HPP
