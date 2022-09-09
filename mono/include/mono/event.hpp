/**
 * @file   event.hpp
 * @author Pratchaya Khansomboon (me@mononerv.dev)
 * @brief  Events
 * @date   2022-05-03
 *
 * @copyright Copyright (c) 2022
 */
#ifndef MONO_EVENT_HPP
#define MONO_EVENT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

#include "common.hpp"
#include "keyboard.hpp"

namespace mno {
enum class event_category : std::uint8_t {
    none        = set_bit(0),
    application = set_bit(1),
    window      = set_bit(2),
    buffer      = set_bit(3),
    keyboard    = set_bit(4),
    mouse       = set_bit(5),
};

// TODO: draw_event
enum class event_type : std::uint32_t {
    none = 0,
    // application
    drop, update, draw,
    // window
    window_resize,  window_move, window_focus, window_icon,
    window_maximize,
    // buffer
    buffer_resize, content_scale,
    // mouse
    mouse_enter, mouse_leave,
    mouse_move,  mouse_press, mouse_release, mouse_wheel,
    // keyboard
    key_down, key_up, key_typed,
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

    auto size() const -> std::size_t { return m_paths.size(); }
    auto paths() const -> std::vector<std::string> { return m_paths; }

  private:
    std::vector<std::string> m_paths;
};

class update_event : public event {
  public:
    update_event(mno::f64 const& time, mno::f64 const& delta)
        : event(event_type::update, event_category::application),
          m_time(time), m_delta(delta) {}

    auto name() const -> std::string override { return "update_event"; }
    auto str() const -> std::string override {
        std::string str{name() + " { "};
        str += "time: "  + std::to_string(m_time) + ", ";
        str += "delta: " + std::to_string(m_delta) + " }";
        return str;
    }

    auto time()  const -> mno::f64 { return m_time;  }
    auto delta() const -> mno::f64 { return m_delta; }

  private:
    mno::f64 m_time;
    mno::f64 m_delta;
};

class draw_event : public event {
  public:
    draw_event(mno::f64 const& time, mno::f64 const& delta)
        : event(event_type::update, event_category::application),
          m_time(time), m_delta(delta) {}

    auto name() const -> std::string override { return "draw_event"; }
    auto str() const -> std::string override {
        std::string str{name() + " { "};
        str += "time: "  + std::to_string(m_time)  + ", ";
        str += "delta: " + std::to_string(m_delta) + " }";
        return str;
    }

    auto time()  const -> mno::f64 { return m_time;  }
    auto delta() const -> mno::f64 { return m_delta; }

  private:
    mno::f64 m_time;
    mno::f64 m_delta;
};

class window_resize_event : public event {
  public:
    window_resize_event(std::int32_t const& width, int32_t const& height)
        : event(event_type::window_resize, event_category::window),
          m_width(width), m_height(height) {}

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
    window_move_event(std::int32_t const& x, std::int32_t const& y)
        : event(event_type::window_move, event_category::window),
          m_x(x), m_y(y) {}

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

    auto name() const -> std::string override {
        return "window_focus_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "focus: ";
        str += (m_focus ? "true" : "false") + " }"s;
        return str;
    }

    auto is_focus() const -> bool { return m_focus; }

  private:
    bool m_focus;
};

class window_icon_event : public event {
  public:
    window_icon_event(bool const& is_icon)
        : event(event_type::window_icon, event_category::window),
          m_is_icon(is_icon) {}

    auto name() const -> std::string override {
        return "window_icon_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "iconified: ";
        str += (m_is_icon ? "true" : "false") + " }"s;
        return str;
    }

    auto is_icon() const -> bool { return m_is_icon; }

  private:
    bool m_is_icon;
};

class window_maximize_event : public event {
  public:
    window_maximize_event(bool const& is_maximize)
        : event(event_type::window_maximize, event_category::window),
          m_is_maximize(is_maximize) {}

    auto name() const -> std::string override {
        return "window_maximize_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "maximize: ";
        str += (m_is_maximize ? "true" : "false") + " }"s;
        return str;
    }

    auto is_maximize() const -> bool { return m_is_maximize; }

  private:
    bool m_is_maximize;
};

class buffer_resize_event : public event {
  public:
    buffer_resize_event(std::int32_t const& width, std::int32_t const& height)
        : event(event_type::buffer_resize, event_category::window),
          m_width(width), m_height(height) {}

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

class content_scale_event : public event {
  public:
    content_scale_event(mno::f32 const& x, mno::f32 const& y)
        : event(event_type::content_scale, event_category::buffer),
          m_x(x), m_y(y) {}

    auto name() const -> std::string override {
        return "content_scale_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: " + std::to_string(m_x) + ",";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto xscale() const -> mno::f32 { return m_x; }
    auto yscale() const -> mno::f32 { return m_y; }

  private:
    mno::f32 m_x;
    mno::f32 m_y;
};

class mouse_event : public event {
  public:
    mouse_event(event_type const& type, mno::f64 const& x, mno::f64 const& y)
        : event(type, event_category::mouse), m_x(x), m_y(y) {}

    auto name() const -> std::string override {
        return "mouse_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto x() const -> mno::f64 { return m_x; }
    auto y() const -> mno::f64 { return m_y; }

  protected:
    mno::f64 m_x;
    mno::f64 m_y;
};

class mouse_move_event : public mouse_event {
  public:
    mouse_move_event(mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_move, x, y) {}

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
};

class mouse_press_event : public mouse_event {
  public:
    mouse_press_event(std::int32_t const& button, std::int32_t const& mods,
                      mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_press, x, y),
          m_button(button), m_mods(mods) {}

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

    auto button() const -> std::int32_t { return m_button; }
    auto mods() const -> std::int32_t { return m_mods; }

  private:
    std::int32_t m_button;
    std::int32_t m_mods;
};

class mouse_release_event : public mouse_event {
  public:
    mouse_release_event(std::int32_t const& button, std::int32_t const& mods,
                      mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_release, x, y),
          m_button(button), m_mods(mods) {}

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

    auto button() const -> std::int32_t { return m_button; }
    auto mods() const -> std::int32_t { return m_mods; }

  protected:
    std::int32_t m_button;
    std::int32_t m_mods;
};

class mouse_wheel_event : public mouse_event {
  public:
    mouse_wheel_event(mno::f64 const& dx, mno::f64 const& dy, mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_wheel, x, y),
          m_dx(dx), m_dy(dy) {}

    auto name() const -> std::string override {
        return "mouse_wheel_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "dx: "  + std::to_string(m_dx)  + ", ";
        str += "dy: " + std::to_string(m_dy) + ", ";
        str += "x: " + std::to_string(m_x) + ", ";
        str += "y: " + std::to_string(m_y) + " }";
        return str;
    }

    auto dx() const -> mno::f64 { return m_dx; }
    auto dy() const -> mno::f64 { return m_dy; }

  private:
    mno::f64 m_dx;
    mno::f64 m_dy;
};

class mouse_enter_event : public mouse_event {
  public:
    mouse_enter_event(mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_enter, x, y) {}

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
};

class mouse_leave_event : public mouse_event {
  public:
    mouse_leave_event(mno::f64 const& x, mno::f64 const& y)
        : mouse_event(event_type::mouse_leave, x, y) {}

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
};

class key_down_event : public event {
  public:
    key_down_event(std::int32_t const& key,
                   std::int32_t const& scan,
                   std::int32_t const& mods,
                   bool const& is_repeat = false)
        : event(event_type::key_down, event_category::keyboard),
          m_key(key), m_scan(scan), m_mods(mods), m_is_repeat(is_repeat) {}

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

    auto key()  const -> std::int32_t { return m_key; }
    auto scan() const -> std::int32_t { return m_scan; }
    auto mods() const -> std::int32_t { return m_mods; }
    auto is_repeat() const -> bool { return m_is_repeat; }

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

    auto key()  const -> std::int32_t { return m_key; }
    auto scan() const -> std::int32_t { return m_scan; }
    auto mods() const -> std::int32_t { return m_mods; }

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

    auto name() const -> std::string override {
        return "key_typed_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "code: " + std::to_string(m_code_point) + " }";
        return str;
    }

    auto code() const -> std::uint32_t { return m_code_point; }

  private:
    std::uint32_t m_code_point;
};

}  // namespace mno

#endif // MONO_EVENT_HPP

