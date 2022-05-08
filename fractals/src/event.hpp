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
#include "mono.hpp"

namespace mono {
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
    drop, drops,
    window_resize, window_move, window_focus,
    buffer_resize,
    mouse_move,    mouse_press, mouse_release, mouse_wheel,
    key_down,      key_up,      key_typed
};

class event {
  public:

  public:
    event(event_type const& type, event_category const& category)
        : m_type(type), m_category(category) {}
    virtual ~event() = default;

    auto type() const -> event_type { return m_type; }
    auto category() const -> event_category { return  m_category; }
    virtual auto name() const -> std::string = 0;
    virtual auto str() const -> std::string = 0;

  private:
    [[maybe_unused]]event_type     m_type;
    [[maybe_unused]]event_category m_category;
};

class drop_event : public event {
  public:
    drop_event(std::string const& path)
        : event(event_type::drop, event_category::application),
          m_path(path) {};
    ~drop_event() = default;

    auto name() const -> std::string override {
        return "drop_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "path: \"" + m_path + "\" }";
        return str;
    }

    auto path() -> std::string { return m_path; }

  private:
    std::string m_path;
};

class drops_event : public event {
  public:
    drops_event(std::vector<std::string> const& path)
        : event(event_type::drops, event_category::application),
          m_paths(path) {};
    ~drops_event() = default;

    auto name() const -> std::string override {
        return "drops_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += "size: " + std::to_string(m_paths.size()) + ", ";
        str += "paths: [";
        std::for_each(std::begin(m_paths), std::end(m_paths), [&](auto const& path){
            str += "\"" + path + "\", ";
        });
        str += "] }";
        return str;
    }

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
    mouse_move_event(mono::f64 const& x, mono::f64 const& y)
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

    auto x() -> mono::f64 { return m_x; }
    auto y() -> mono::f64 { return m_y; }

  private:
    mono::f64 m_x;
    mono::f64 m_y;
};

class mouse_press_event : public event {
  public:
    mouse_press_event(std::int32_t const& button, std::int32_t const& mods,
                      mono::f64 const& x, mono::f64 const& y)
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
    auto x() -> mono::f64 { return m_x; }
    auto y() -> mono::f64 { return m_y; }

  private:
    std::int32_t m_button;
    std::int32_t m_mods;
    mono::f64 m_x;
    mono::f64 m_y;
};

class mouse_release_event : public event {
  public:
    mouse_release_event(std::int32_t const& button, std::int32_t const& mods,
                      mono::f64 const& x, mono::f64 const& y)
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
    auto x() -> mono::f64 { return m_x; }
    auto y() -> mono::f64 { return m_y; }

  private:
    std::int32_t m_button;
    std::int32_t m_mods;
    mono::f64 m_x;
    mono::f64 m_y;
};

class mouse_wheel_event : public event {
  public:
    mouse_wheel_event(mono::f64 const& deltax, mono::f64 const& deltay)
        : event(event_type::mouse_wheel, event_category::mouse),
          m_delta_x(deltax), m_delta_y(deltay) {}
    ~mouse_wheel_event() = default;

    auto name() const -> std::string override {
        return "mouse_wheel_event";
    }
    auto str() const -> std::string override {
        using namespace std::string_literals;
        std::string str{name() + " { "};
        str += " delta_x: "  + std::to_string(m_delta_x)  + ",";
        str += " delta_y: " + std::to_string(m_delta_y) + " }";
        return str;
    }

    auto delta_x() -> mono::f64 { return m_delta_x; }
    auto delta_y() -> mono::f64 { return m_delta_y; }

  private:
    mono::f64 m_delta_x;
    mono::f64 m_delta_y;
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
    key_typed_event(std::int32_t const& key,
                   std::int32_t const& scan,
                   std::int32_t const& mods)
        : event(event_type::key_typed, event_category::keyboard),
          m_key(key), m_scan(scan), m_mods(mods) {}
    ~key_typed_event() = default;

    auto name() const -> std::string override {
        return "key_typed_event";
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

}

#endif //FRACTALS_EVENT_HPP

