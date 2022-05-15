/**
 * @file   common.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Common includes.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_COMMON_HPP
#define MONO_COMMON_HPP

#include <cstdint>
#include <memory>

namespace mono {

#define MONO_BIT(x) 1 << x

using f32 = float;          // float 32-bit
using f64 = double;         // float 64-bit

using u8  = std::uint8_t;   // unsigned integer  8-bit
using u16 = std::uint16_t;  // unsinged integer 16-bit
using u32 = std::uint32_t;  // unsigned integer 32-bit
using u64 = std::uint64_t;  // unsinged integer 64-bit

using i8  = std::int8_t;    // integer  8-bit
using i16 = std::int16_t;   // integer 16-bit
using i32 = std::int32_t;   // integer 32-bit
using i64 = std::int64_t;   // integer 64-bit

template <typename T>
using ref = std::shared_ptr<T>;

template <typename T>
using local = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr auto make_ref(Args&&... args) -> ref<T> {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr auto make_local(Args&&... args) -> local<T> {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
}  // namespace mono

#endif // MONO_COMMON_HPP
