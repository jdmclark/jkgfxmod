#pragma once

#include "base/span.hpp"
#include "base/unique_handle.hpp"
#include "gl.hpp"
#include "math/size.hpp"
#include <optional>

namespace jkgm::gl {
    struct texture_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using texture = unique_handle<texture_traits>;
    using texture_view = unique_handle_view<texture_traits>;

    enum class mag_filter : int_type { nearest = 0x2600, linear = 0x2601 };

    enum class min_filter : int_type {
        nearest = 0x2600,
        linear = 0x2601,
        nearest_mipmap_nearest = 0x2700,
        linear_mipmap_nearest = 0x2701,
        nearest_mipmap_linear = 0x2702,
        linear_mipmap_linear = 0x2703
    };

    enum class texture_bind_target : enum_type {
        texture_2d = 0x0DE1,
        texture_2d_multisample = 0x9100,
        cube_map = 0x8513,
        cube_map_positive_x = 0x8515,
        cube_map_negative_x = 0x8516,
        cube_map_positive_y = 0x8517,
        cube_map_negative_y = 0x8518,
        cube_map_positive_z = 0x8519,
        cube_map_negative_z = 0x851a
    };

    enum class texture_compare_mode : enum_type { compare_ref_to_texture = 0x884E, none = 0x0 };

    enum class texture_direction : enum_type { s = 0x2802, t = 0x2803, r = 0x8072 };

    enum class texture_internal_format : int_type {
        rgb = 0x1907,
        rgba = 0x1908,
        rgba16f = 0x881A,
        rgba32f = 0x8814,
        r16f = 0x822D,
        rg32f = 0x8230,
        depth_component_16 = 0x81A5
    };

    enum class texture_pixel_format : enum_type {
        rgb = 0x1907,
        rgba = 0x1908,
        rg = 0x8227,
        bgra = 0x80E1,
        depth_component = 0x1902
    };

    enum class texture_pixel_type : enum_type { uint8 = 0x1401, float32 = 0x1406 };

    enum class texture_wrap_mode : enum_type {
        clamp_to_edge = 0x812F,
        clamp_to_border = 0x812D,
        mirrored_repeat = 0x8370,
        repeat = 0x2901
    };

    constexpr texture_view default_texture = texture_view(0U);

    void bind_texture(texture_bind_target target, texture_view tex);

    void generate_mipmap(texture_bind_target target);

    void set_active_texture_unit(int unit);

    void set_texture_compare_mode(texture_bind_target target, texture_compare_mode mode);
    void set_texture_compare_function(texture_bind_target target, comparison_function func);
    void set_texture_max_level(texture_bind_target target, int max_level);
    void set_texture_mag_filter(texture_bind_target target, mag_filter filter);
    void set_texture_min_filter(texture_bind_target target, min_filter filter);
    void set_texture_wrap_mode(texture_bind_target target,
                               texture_direction dir,
                               texture_wrap_mode mode);

    void tex_image_2d(texture_bind_target target,
                      int level,
                      texture_internal_format int_fmt,
                      size<2, int> dimensions,
                      texture_pixel_format pix_fmt,
                      texture_pixel_type pix_type,
                      span<char const> data);

    void tex_sub_image_2d(texture_bind_target target,
                          int level,
                          box<2, int> region,
                          texture_pixel_format pix_fmt,
                          texture_pixel_type pix_type,
                          span<char const> data);

    void tex_image_2d_multisample(texture_bind_target target,
                                  size_type num_samples,
                                  texture_internal_format int_fmt,
                                  size<2, int> dimensions,
                                  bool fixed_sample_locations);
}
