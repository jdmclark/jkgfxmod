#include "renderer_screen_basic.hpp"
#include "base/log.hpp"
#include "math/colors.hpp"

jkgm::render_depthbuffer::render_depthbuffer(size<2, int> dims)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_renderbuffer(rbo);
    gl::renderbuffer_storage(gl::renderbuffer_format::depth, dims);
}

jkgm::render_buffer::render_buffer(size<2, int> dims, render_depthbuffer *rbo)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    // Set up color texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba16f,
                     dims,
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::float32,
                     span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

    // Set up real depth buffer:
    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo->rbo);

    // Finish:
    gl::draw_buffers(gl::draw_buffer::color0);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::render_gbuffer::render_gbuffer(size<2, int> dims, render_depthbuffer *rbo)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    // Set up color texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, color_tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba16f,
                     dims,
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::float32,
                     span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, color_tex, 0);

    // Set up emissive texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, emissive_tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba16f,
                     dims,
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::float32,
                     span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color1, emissive_tex, 0);

    // Set up normal texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, normal_tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgb16f,
                     dims,
                     gl::texture_pixel_format::rgb,
                     gl::texture_pixel_type::float32,
                     make_span((char const *)nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d, gl::min_filter::linear);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_border_color(gl::texture_bind_target::texture_2d,
                                 color(0.0f, 0.0f, 1.0f, std::numeric_limits<float>::lowest()));
    gl::framebuffer_texture(gl::framebuffer_bind_target::any,
                            gl::framebuffer_attachment::color2,
                            normal_tex,
                            /*level*/ 0);

    // Set up depth texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, depth_tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::r16f,
                     dims,
                     gl::texture_pixel_format::red,
                     gl::texture_pixel_type::float32,
                     make_span((char const *)nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d, gl::min_filter::linear);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_border_color(gl::texture_bind_target::texture_2d,
                                 color(0.0f, 0.0f, 1.0f, std::numeric_limits<float>::lowest()));
    gl::framebuffer_texture(gl::framebuffer_bind_target::any,
                            gl::framebuffer_attachment::color3,
                            depth_tex,
                            /*level*/ 0);

    // Set up real depth buffer:
    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo->rbo);

    // Finish:
    gl::draw_buffers(gl::draw_buffer::color0,
                     gl::draw_buffer::color1,
                     gl::draw_buffer::color2,
                     gl::draw_buffer::color3);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::renderer_screen_basic::renderer_screen_basic(size<2, int> screen_res)
    : shared_depthbuffer(screen_res)
    , screen_buffer(screen_res, &shared_depthbuffer)
    , screen_gbuffer(screen_res, &shared_depthbuffer)
{
}

void jkgm::renderer_screen_basic::begin_frame()
{
    gl::set_viewport(screen_buffer.viewport);

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
    gl::clear_buffer_color(0, color::zero());
    gl::clear_buffer_depth(1.0f);
}

void jkgm::renderer_screen_basic::begin_opaque_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_gbuffer.fbo);
    gl::clear_buffer_color(0, color::zero());
    gl::clear_buffer_color(1, color::zero());
    gl::clear_buffer_color(2, color::zero());
    gl::clear_buffer_color(3, color::fill(1.0f));
    gl::clear_buffer_depth(1.0f);
}

void jkgm::renderer_screen_basic::end_opaque_pass() {}

jkgm::gl::texture_view jkgm::renderer_screen_basic::get_resolved_color_texture()
{
    return screen_gbuffer.color_tex;
}

jkgm::gl::texture_view jkgm::renderer_screen_basic::get_resolved_emissive_texture()
{
    return screen_gbuffer.emissive_tex;
}

jkgm::gl::texture_view jkgm::renderer_screen_basic::get_resolved_normal_texture()
{
    return screen_gbuffer.normal_tex;
}

jkgm::gl::texture_view jkgm::renderer_screen_basic::get_resolved_depth_texture()
{
    return screen_gbuffer.depth_tex;
}

void jkgm::renderer_screen_basic::begin_compose_opaque_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
    gl::clear_buffer_color(0, color::zero());
}

void jkgm::renderer_screen_basic::end_compose_opaque_pass() {}

void jkgm::renderer_screen_basic::begin_transparency_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
}

void jkgm::renderer_screen_basic::end_transparency_pass() {}

void jkgm::renderer_screen_basic::end_frame() {}

jkgm::gl::texture_view jkgm::renderer_screen_basic::get_resolved_final_texture()
{
    return screen_buffer.tex;
}