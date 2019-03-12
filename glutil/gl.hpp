#pragma once

#include "base/flag_set.hpp"
#include "gl_types.hpp"
#include "math/box.hpp"
#include "math/color.hpp"

namespace jkgm::gl {
    enum class blend_function : enum_type {
        zero = 0x0,
        one = 0x1,
        one_minus_source_alpha = 0x0303
    };

    enum class capability : enum_type {
        alpha_test = 0x0BC0,
        blend = 0x0BE2,
        cull_face = 0x0B44,
        depth_test = 0x0B71,
        framebuffer_srgb = 0x8DB9
    };

    enum class face_mode : enum_type { front = 0x0404, back = 0x0405, front_and_back = 0x0408 };

    enum class comparison_function : enum_type {
        never = 0x0200,
        less = 0x0201,
        equal = 0x0202,
        less_equal = 0x0203,
        greater = 0x0204,
        not_equal = 0x0205,
        greater_equal = 0x0206,
        always = 0x0207
    };

    enum class polygon_mode : enum_type { point = 0x1b00, line = 0x1b01, fill = 0x1b02 };

    enum class clear_flag : bitfield_type { color = 0x00004000, depth = 0x00000100 };
    using clear_flags = flag_set<clear_flag>;

    void enable(capability cap);
    void disable(capability cap);

    void clear(clear_flags cf);

    void set_alpha_function(comparison_function func, float ref);
    void set_blend_function(blend_function sfactor, blend_function dfactor);
    void set_clear_color(color c);
    void set_depth_function(comparison_function func);
    void set_depth_mask(bool enable);
    void set_face_cull_mode(face_mode mode);
    void set_polygon_mode(face_mode fm, polygon_mode pm);
    void set_viewport(box<2, int> vp);

    void log_errors();
}
