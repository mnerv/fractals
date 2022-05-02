#include "input.hpp"
#include "GLFW/glfw3.h"

namespace mono::state {
auto key::make(std::int32_t const& key) -> ref<state::key> {
    return make_ref<state::key>(key);
}

auto key::update(const bool &state) -> void {
    states[1] = states[0];
    states[0] = state;
}

auto key::is_clicked(key const& key) -> bool {
    return key.states[0] == GLFW_PRESS && key.states[1] == GLFW_RELEASE;
}
auto key::is_press(key const& key) -> bool {
    return key.states[0] == GLFW_PRESS;
}
auto key::is_release(key const& key) -> bool {
    return key.states[0] == GLFW_RELEASE;
}

auto key::is_clicked(ref<key> const& key) -> bool {
    return key::is_clicked(*key);
}
auto key::is_press(ref<key> const& key) -> bool {
    return key::is_press(*key);
}
auto key::is_release(ref<key> const& key) -> bool {
    return key::is_release(*key);
}
}