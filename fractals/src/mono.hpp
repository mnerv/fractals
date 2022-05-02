/**
 * @file   mono.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Common includes.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_MONO_HPP
#define FRACTALS_MONO_HPP

#include <memory>

namespace mono {
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

}

#endif //FRACTALS_MONO_HPP
