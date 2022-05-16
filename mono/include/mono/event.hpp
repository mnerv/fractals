/**
 * @file   event.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Events
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef FRACTALS_EVENT_HPP
#define FRACTALS_EVENT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

#include "common.hpp"
#include "keyboard.hpp"

namespace mno {
enum class event_category : std::uint8_t {
    none        = MONO_BIT(0),
    application = MONO_BIT(1),
    window      = MONO_BIT(2),
    buffer      = MONO_BIT(3),
    keyboard    = MONO_BIT(4),
    mouse       = MONO_BIT(5)
};

enum class event_type : std::uint32_t {
    none = 0,
    drop,
    window_resize, window_move, window_focus,
    buffer_resize,
    mouse_enter,   mouse_leave,
    mouse_move,    mouse_press, mouse_release, mouse_wheel,
    key_down,      key_up,      key_typed
};

class event {
  public:
    event(event_type const& type, event_category const& category)
        : m_type(type), m_category(category) {}
    virtual ~event() = default;

    constexpr auto type() const -> event_type { return m_type; }
    constexpr auto category() const -> event_category { return  m_category; }
    virtual auto name() const -> std::string = 0;
    virtual auto str() const -> std::string = 0;

  private:
    event_type     m_type{event_type::none};
    event_category m_category{event_category::none};
};

class drop_event : public event {
  public:
    drop_event(std::vector<std::string> const& paths)
        : event(event_type::drop, event_category::application),
          m_paths(paths) {};
    ~drop_event() = default;

    auto name() const -> std::string override {
        return "drop_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "size: " + std::to_string(m_paths.size()) + ", ";
        str += "paths: [ ";
        for (std::size_t i = 0; i < m_paths.size(); i++) {
            str += "\"" + m_paths.at(i) + "\"";
            if (i < m_paths.size() - 1) str += ", ";
        }
        str += " ] }";
        return str;
    }

    auto size() -> std::size_t { return m_paths.size(); }
    auto paths() -> std::vector<std::string> { return m_paths; }

  private:
    std::vector<std::string> m_paths;
};

class window_resize_event : public event {
  public:
    window_resize_event(std::int32_t const& width, int32_t const& height)
        : event(event_type::window_resize, event_category::window),
          m_width(width), m_height(height) {}
    ~window_resize_event() = default;

    auto name() const -> std::string override {
        return "window_resize_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "width: "  + std::to_string(m_width)  + ", ";
        str += "height: " + std::to_string(m_height) + " }";
        return str;
    }

    auto width() const -> std::int32_t { return m_width; }
    auto height() const -> std::int32_t { return m_height; }

  private:
    std::int32_t m_width;
    std::int32_t m_height;
};

class window_move_event : public event {
  public:
    window_move_event(std::int32_t const& x, int32_t const& y)
        : event(event_type::window_move, event_category::window),
          m_x(x), m_y(y) {}
    ~window_move_event() = default;

    auto name() const -> std::string override {
        return "window_move_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: "  + std::to_string(m_x)  + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto x() const -> std::int32_t { return m_x; }
    auto y() const -> std::int32_t { return m_y; }

  private:
    std::int32_t m_x;
    std::int32_t m_y;
};

class window_focus_event : public event {
  public:
    window_focus_event(bool const& is_focus)
        : event(event_type::window_focus, event_category::window),
          m_focus(is_focus) {}
    ~window_focus_event() = default;

    auto name() const -> std::string override {
        return "window_focus_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "focus: ";
        str += m_focus ? "true" : "false" + " }"s;
        return str;
    }

    auto is_focus() const -> bool { return m_focus; }

  private:
    bool m_focus;
};

class buffer_resize_event : public event {
  public:
    buffer_resize_event(std::int32_t const& width, int32_t const& height)
        : event(event_type::buffer_resize, event_category::window),
          m_width(width), m_height(height) {}
    ~buffer_resize_event() = default;

    auto name() const -> std::string override {
        return "buffer_resize_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "width: "  + std::to_string(m_width)  + ",";
        str += "height: " + std::to_string(m_height) + " }";
        return str;
    }

    auto width() const -> std::int32_t { return m_width; }
    auto height() const -> std::int32_t { return m_height; }

  private:
    std::int32_t m_width;
    std::int32_t m_height;
};

class mouse_move_event : public event {
  public:
    mouse_move_event(mno::f64 const& x, mno::f64 const& y)
        : event(event_type::mouse_move, event_category::mouse),
          m_x(x), m_y(y) {}
    ~mouse_move_event() = default;

    auto name() const -> std::string override {
        return "mouse_move_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto x() -> mno::f64 { return m_x; }
    auto y() -> mno::f64 { return m_y; }

  private:
    mno::f64 m_x;
    mno::f64 m_y;
};

class mouse_press_event : public event {
  public:
    mouse_press_event(std::int32_t const& button, std::int32_t const& mods,
                      mno::f64 const& x, mno::f64 const& y)
        : event(event_type::mouse_press, event_category::mouse),
          m_button(button), m_mods(mods), m_x(x), m_y(y) {}
    ~mouse_press_event() = default;

    auto name() const -> std::string override {
        return "mouse_press_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "button: " + std::to_string(m_button) + ", ";
        str += "mods: "   + std::to_string(m_mods)   + ", ";
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto button() -> std::int32_t { return m_button; }
    auto mods() -> std::int32_t { return m_mods; }
    auto x() -> mno::f64 { return m_x; }
    auto y() -> mno::f64 { return m_y; }

  private:
    std::int32_t m_button;
    std::int32_t m_mods;
    mno::f64 m_x;
    mno::f64 m_y;
};

class mouse_release_event : public event {
  public:
    mouse_release_event(std::int32_t const& button, std::int32_t const& mods,
                      mno::f64 const& x, mno::f64 const& y)
        : event(event_type::mouse_release, event_category::mouse),
          m_button(button), m_mods(mods), m_x(x), m_y(y) {}
    ~mouse_release_event() = default;

    auto name() const -> std::string override {
        return "mouse_release_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "button: " + std::to_string(m_button) + ", ";
        str += "mods: "   + std::to_string(m_mods)   + ", ";
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto button() -> std::int32_t { return m_button; }
    auto mods() -> std::int32_t { return m_mods; }
    auto x() -> mno::f64 { return m_x; }
    auto y() -> mno::f64 { return m_y; }

  private:
    std::int32_t m_button;
    std::int32_t m_mods;
    mno::f64 m_x;
    mno::f64 m_y;
};

class mouse_wheel_event : public event {
  public:
    mouse_wheel_event(mno::f64 const& dx, mno::f64 const& dy)
        : event(event_type::mouse_wheel, event_category::mouse),
          m_dx(dx), m_dy(dy) {}
    ~mouse_wheel_event() = default;

    auto name() const -> std::string override {
        return "mouse_wheel_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "dx: "  + std::to_string(m_dx)  + ", ";
        str += "dy: " + std::to_string(m_dy) + " }";
        return str;
    }

    auto dx() -> mno::f64 { return m_dx; }
    auto dy() -> mno::f64 { return m_dy; }

  private:
    mno::f64 m_dx;
    mno::f64 m_dy;
};

class mouse_enter_event : public event {
  public:
    mouse_enter_event(mno::f64 const& x, mno::f64 const& y)
        : event(event_type::mouse_enter, event_category::mouse),
          m_x(x), m_y(y) {}
    ~mouse_enter_event() = default;

    auto name() const -> std::string override {
        return "mouse_enter_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto x() -> mno::f64 { return m_x; }
    auto y() -> mno::f64 { return m_y; }

  private:
    mno::f64 m_x;
    mno::f64 m_y;
};

class mouse_leave_event : public event {
  public:
    mouse_leave_event(mno::f64 const& x, mno::f64 const& y)
        : event(event_type::mouse_leave, event_category::mouse),
          m_x(x), m_y(y) {}
    ~mouse_leave_event() = default;

    auto name() const -> std::string override {
        return "mouse_leave_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto x() -> mno::f64 { return m_x; }
    auto y() -> mno::f64 { return m_y; }

  private:
    mno::f64 m_x;
    mno::f64 m_y;
};

class key_down_event : public event {
  public:
    key_down_event(std::int32_t const& key,
                   std::int32_t const& scan,
                   std::int32_t const& mods,
                   bool const& is_repeat = false)
        : event(event_type::key_down, event_category::keyboard),
          m_key(key), m_scan(scan), m_mods(mods), m_is_repeat(is_repeat) {}
    ~key_down_event() = default;

    auto name() const -> std::string override {
        return "key_down_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "key: "  + std::to_string(m_key)  + ", ";
        str += "scan: " + std::to_string(m_scan) + ", ";
        str += "mods: " + std::to_string(m_mods) + ", ";
        str += "is_repeat: "s + (m_is_repeat ? "true" : "false") + " }"s;
        return str;
    }

    auto key()  -> std::int32_t { return m_key; }
    auto scan() -> std::int32_t { return m_scan; }
    auto mods() -> std::int32_t { return m_mods; }
    auto is_repeat() -> bool { return m_is_repeat; }

  private:
    std::int32_t m_key;
    std::int32_t m_scan;
    std::int32_t m_mods;
    bool         m_is_repeat;
};

class key_up_event : public event {
  public:
    key_up_event(std::int32_t const& key,
                   std::int32_t const& scan,
                   std::int32_t const& mods)
        : event(event_type::key_up, event_category::keyboard),
          m_key(key), m_scan(scan), m_mods(mods) {}
    ~key_up_event() = default;

    auto name() const -> std::string override {
        return "key_up_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "key: "  + std::to_string(m_key)  + ", ";
        str += "scan: " + std::to_string(m_scan) + ", ";
        str += "mods: " + std::to_string(m_mods) + " }";
        return str;
    }

    auto key()  -> std::int32_t { return m_key; }
    auto scan() -> std::int32_t { return m_scan; }
    auto mods() -> std::int32_t { return m_mods; }

  private:
    std::int32_t m_key;
    std::int32_t m_scan;
    std::int32_t m_mods;
};

class key_typed_event : public event {
  public:
    key_typed_event(std::uint32_t const& code_point)
        : event(event_type::key_typed, event_category::keyboard),
          m_code_point(code_point) {}
    ~key_typed_event() = default;

    auto name() const -> std::string override {
        return "key_typed_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "code: " + std::to_string(m_code_point) + " }";
        return str;
    }

    auto code() -> std::uint32_t { return m_code_point; }

  private:
    std::uint32_t m_code_point;
};

}  // namespace mno

#endif // FRACTALS_EVENT_HPP

