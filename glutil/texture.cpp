#include "texture.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::texture_traits::create()
{
    GLuint rv = 0;
    glGenTextures(1, &rv);
    return rv;
}

void jkgm::gl::texture_traits::destroy(GLuint id)
{
    glDeleteTextures(1, &id);
}

void jkgm::gl::bind_texture(texture_bind_target target, texture_view tex)
{
    glBindTexture(static_cast<GLenum>(target), *tex);
}

void jkgm::gl::generate_mipmap(texture_bind_target target)
{
    glGenerateMipmap(static_cast<GLenum>(target));
}

void jkgm::gl::set_active_texture_unit(int unit)
{
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
}

void jkgm::gl::set_texture_compare_mode(texture_bind_target target, texture_compare_mode mode)
{
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_COMPARE_MODE, static_cast<GLint>(mode));
}

void jkgm::gl::set_texture_compare_function(texture_bind_target target, comparison_function func)
{
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(func));
}

void jkgm::gl::set_texture_max_level(texture_bind_target target, int max_level)
{
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MAX_LEVEL, max_level);
}

void jkgm::gl::set_texture_mag_filter(texture_bind_target target, mag_filter filter)
{
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
}

void jkgm::gl::set_texture_min_filter(texture_bind_target target, min_filter filter)
{
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
}

void jkgm::gl::set_texture_wrap_mode(texture_bind_target target,
                                     texture_direction dir,
                                     texture_wrap_mode mode)
{
    glTexParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(dir), static_cast<GLint>(mode));
}

void jkgm::gl::tex_image_2d(texture_bind_target target,
                            int level,
                            texture_internal_format int_fmt,
                            size<2, int> dimensions,
                            texture_pixel_format pix_fmt,
                            texture_pixel_type pix_type,
                            span<char const> data)
{
    glTexImage2D(static_cast<GLenum>(target),
                 level,
                 static_cast<GLint>(int_fmt),
                 get<x>(dimensions),
                 get<y>(dimensions),
                 /*per standard, must always be 0*/ 0,
                 static_cast<GLenum>(pix_fmt),
                 static_cast<GLenum>(pix_type),
                 data.data());
}

void jkgm::gl::tex_sub_image_2d(texture_bind_target target,
                                int level,
                                box<2, int> region,
                                texture_pixel_format pix_fmt,
                                texture_pixel_type pix_type,
                                span<char const> data)
{
    auto rgn_size = region.size();
    glTexSubImage2D(static_cast<GLenum>(target),
                    level,
                    get<x>(region.start),
                    get<y>(region.start),
                    get<x>(rgn_size),
                    get<y>(rgn_size),
                    static_cast<GLenum>(pix_fmt),
                    static_cast<GLenum>(pix_type),
                    data.data());
}

void jkgm::gl::tex_image_2d_multisample(texture_bind_target target,
                                        size_type num_samples,
                                        texture_internal_format int_fmt,
                                        size<2, int> dimensions,
                                        bool fixed_sample_locations)
{
    glTexImage2DMultisample(static_cast<GLenum>(target),
                            num_samples,
                            static_cast<GLenum>(int_fmt),
                            get<x>(dimensions),
                            get<y>(dimensions),
                            fixed_sample_locations ? GL_TRUE : GL_FALSE);
}

namespace jkgm::gl {
    static_assert(mag_filter::nearest == mag_filter(GL_NEAREST));
    static_assert(mag_filter::linear == mag_filter(GL_LINEAR));

    static_assert(min_filter::nearest == min_filter(GL_NEAREST));
    static_assert(min_filter::linear == min_filter(GL_LINEAR));
    static_assert(min_filter::nearest_mipmap_nearest == min_filter(GL_NEAREST_MIPMAP_NEAREST));
    static_assert(min_filter::linear_mipmap_nearest == min_filter(GL_LINEAR_MIPMAP_NEAREST));
    static_assert(min_filter::nearest_mipmap_linear == min_filter(GL_NEAREST_MIPMAP_LINEAR));
    static_assert(min_filter::linear_mipmap_linear == min_filter(GL_LINEAR_MIPMAP_LINEAR));

    static_assert(texture_bind_target::texture_2d == texture_bind_target(GL_TEXTURE_2D));
    static_assert(texture_bind_target::texture_2d_multisample ==
                  texture_bind_target(GL_TEXTURE_2D_MULTISAMPLE));
    static_assert(texture_bind_target::cube_map == texture_bind_target(GL_TEXTURE_CUBE_MAP));
    static_assert(texture_bind_target::cube_map_positive_x ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_POSITIVE_X));
    static_assert(texture_bind_target::cube_map_negative_x ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_NEGATIVE_X));
    static_assert(texture_bind_target::cube_map_positive_y ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_POSITIVE_Y));
    static_assert(texture_bind_target::cube_map_negative_y ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y));
    static_assert(texture_bind_target::cube_map_positive_z ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_POSITIVE_Z));
    static_assert(texture_bind_target::cube_map_negative_z ==
                  texture_bind_target(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));

    static_assert(texture_compare_mode::compare_ref_to_texture ==
                  texture_compare_mode(GL_COMPARE_REF_TO_TEXTURE));
    static_assert(texture_compare_mode::none == texture_compare_mode(GL_NONE));

    static_assert(texture_direction::s == texture_direction(GL_TEXTURE_WRAP_S));
    static_assert(texture_direction::t == texture_direction(GL_TEXTURE_WRAP_T));
    static_assert(texture_direction::r == texture_direction(GL_TEXTURE_WRAP_R));

    static_assert(texture_internal_format::rgb == texture_internal_format(GL_RGB));
    static_assert(texture_internal_format::rgba == texture_internal_format(GL_RGBA));
    static_assert(texture_internal_format::rgba32f == texture_internal_format(GL_RGBA32F));
    static_assert(texture_internal_format::rg32f == texture_internal_format(GL_RG32F));
    static_assert(texture_internal_format::depth_component_16 ==
                  texture_internal_format(GL_DEPTH_COMPONENT16));

    static_assert(texture_pixel_format::rgb == texture_pixel_format(GL_RGB));
    static_assert(texture_pixel_format::rgba == texture_pixel_format(GL_RGBA));
    static_assert(texture_pixel_format::rg == texture_pixel_format(GL_RG));
    static_assert(texture_pixel_format::bgra == texture_pixel_format(GL_BGRA));
    static_assert(texture_pixel_format::depth_component ==
                  texture_pixel_format(GL_DEPTH_COMPONENT));

    static_assert(texture_pixel_type::uint8 == texture_pixel_type(GL_UNSIGNED_BYTE));
    static_assert(texture_pixel_type::float32 == texture_pixel_type(GL_FLOAT));

    static_assert(texture_wrap_mode::clamp_to_edge == texture_wrap_mode(GL_CLAMP_TO_EDGE));
    static_assert(texture_wrap_mode::clamp_to_border == texture_wrap_mode(GL_CLAMP_TO_BORDER));
    static_assert(texture_wrap_mode::mirrored_repeat == texture_wrap_mode(GL_MIRRORED_REPEAT));
    static_assert(texture_wrap_mode::repeat == texture_wrap_mode(GL_REPEAT));
}
