/**
 * @file   input.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Input interface for ease of use for handling click and press events.
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_INPUT_HPP
#define FRACTALS_INPUT_HPP

#include <cstdint>
#include <cstddef>
#include "mono.hpp"

namespace mono::state {
constexpr std::size_t STATE_SIZE = 2;

struct key {
    std::int32_t value;
    bool states[STATE_SIZE]{false, false};

    explicit key(std::int32_t const& key_value) : value(key_value) {}
    auto update(bool const& state) -> void;

    static auto make(std::int32_t const& key) -> ref<state::key>;
    static auto is_clicked(key const& key) -> bool;
    static auto is_clicked(ref<key> const& key) -> bool;

    static auto is_press(key const& key) -> bool;
    static auto is_press(ref<key> const& key) -> bool;

    static auto is_release(key const& key) -> bool;
    static auto is_release(ref<key> const& key) -> bool;
};

}

#endif //FRACTALS_INPUT_HPP
