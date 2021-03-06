#pragma once

#include <ncurses.h>

namespace bookwyrm::tui {

    enum key : int {
        // clang-format off
        f1               = KEY_F(1),
        f2               = KEY_F(2),
        f3               = KEY_F(3),
        f4               = KEY_F(4),
        f5               = KEY_F(5),
        f6               = KEY_F(6),
        f7               = KEY_F(7),
        f8               = KEY_F(8),
        f9               = KEY_F(9),
        f10              = KEY_F(10),
        f11              = KEY_F(11),
        f12              = KEY_F(12),
        insert           = KEY_IL,
        del              = KEY_DC,
        home             = KEY_HOME,
        end              = KEY_END,
        pgup             = KEY_PPAGE,
        pgdn             = KEY_NPAGE,
        arrow_up         = KEY_UP,
        arrow_down       = KEY_DOWN,
        arrow_left       = KEY_LEFT,
        arrow_right      = KEY_RIGHT,
        mouse_left       = 0,
        mouse_right      = 0,
        mouse_middle     = 0,
        mouse_release    = 0,
        mouse_wheel_up   = 0,
        mouse_wheel_down = 0,
        resize           = KEY_RESIZE,

        /*
         * Below numbers are the return values of getch()
         * for the listed key. I do not know if these are
         * portable.
         */
        tab              = 9,
        enter            = 10,
        space            = 32,
        // clang-format on
    };

} // namespace bookwyrm::tui
