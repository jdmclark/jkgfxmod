#include "renderer_screen_msaa.hpp"
#include "base/log.hpp"
#include "math/colors.hpp"

jkgm::render_depthbuffer_msaa::render_depthbuffer_msaa(size<2, int> dims, int num_samples)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_renderbuffer(rbo);
    gl::renderbuffer_storage_multisample(num_samples, gl::renderbuffer_format::depth, dims);
}

jkgm::render_buffer_msaa::render_buffer_msaa(size<2, int> dims,
                                             int num_samples,
                                             render_depthbuffer_msaa *rbo)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    // Set up color texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, tex);
    gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                 num_samples,
                                 gl::texture_internal_format::rgba16f,
                                 dims,
                                 /*fixed sample locations*/ true);
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d_multisample, 0U);
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
        LOG_ERROR("Failed to create msaa framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::render_gbuffer_msaa::render_gbuffer_msaa(size<2, int> dims,
                                               int num_samples,
                                               render_depthbuffer_msaa *rbo)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    // Set up color texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, color_tex);
    gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                 num_samples,
                                 gl::texture_internal_format::rgba16f,
                                 dims,
                                 /*fixed sample locations*/ true);
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d_multisample, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, color_tex, 0);

    // Set up emissive texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, emissive_tex);
    gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                 num_samples,
                                 gl::texture_internal_format::rgba16f,
                                 dims,
                                 /*fixed sample locations*/ true);
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d_multisample, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color1, emissive_tex, 0);

    // Set up depth normal texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, depth_nrm_tex);
    gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                 num_samples,
                                 gl::texture_internal_format::rgba32f,
                                 dims,
                                 /*fixed sample locations*/ true);
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d_multisample, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d_multisample,
                               gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d_multisample,
                               gl::min_filter::linear);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d_multisample,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d_multisample,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_border);
    gl::set_texture_border_color(gl::texture_bind_target::texture_2d_multisample,
                                 color(0.0f, 0.0f, 1.0f, std::numeric_limits<float>::lowest()));
    gl::framebuffer_texture(gl::framebuffer_bind_target::any,
                            gl::framebuffer_attachment::color2,
                            depth_nrm_tex,
                            /*level*/ 0);

    // Set up real depth buffer:
    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo->rbo);

    // Finish:
    gl::draw_buffers(gl::draw_buffer::color0, gl::draw_buffer::color1, gl::draw_buffer::color2);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create msaa gbuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::msaa_resolve_buffer::msaa_resolve_buffer(size<2, int> dims,
                                               gl::texture_internal_format tif,
                                               gl::texture_pixel_format tpf,
                                               gl::texture_pixel_type tpt)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    // Set up color texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     tif,
                     dims,
                     tpf,
                     tpt,
                     span<char const>(nullptr, 0U));
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
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

    // Finish:
    gl::draw_buffers(gl::draw_buffer::color0);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create msaa resolve buffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::renderer_screen_msaa::renderer_screen_msaa(size<2, int> screen_res,
                                                 int num_samples,
                                                 std::optional<float> min_sample_factor)
    : shared_depthbuffer(screen_res, num_samples)
    , screen_buffer(screen_res, num_samples, &shared_depthbuffer)
    , screen_gbuffer(screen_res, num_samples, &shared_depthbuffer)
    , gbuffer_color_resolve(screen_res,
                            gl::texture_internal_format::rgba16f,
                            gl::texture_pixel_format::rgba,
                            gl::texture_pixel_type::float32)
    , gbuffer_emissive_resolve(screen_res,
                               gl::texture_internal_format::rgba16f,
                               gl::texture_pixel_format::rgba,
                               gl::texture_pixel_type::float32)
    , gbuffer_depth_nrm_resolve(screen_res,
                                gl::texture_internal_format::rgba32f,
                                gl::texture_pixel_format::rgba,
                                gl::texture_pixel_type::float32)
    , buffer_resolve(screen_res,
                     gl::texture_internal_format::rgba16f,
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::float32)
{
    if(min_sample_factor.has_value()) {
        gl::enable(gl::capability::sample_shading);
        gl::set_min_sample_shading_factor(*min_sample_factor);
    }
}

void jkgm::renderer_screen_msaa::begin_frame()
{
    gl::set_viewport(screen_buffer.viewport);

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
    gl::clear_buffer_color(0, color::zero());
    gl::clear_buffer_depth(1.0f);
}

void jkgm::renderer_screen_msaa::begin_opaque_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_gbuffer.fbo);
    gl::clear_buffer_color(0, color::zero());
    gl::clear_buffer_color(1, color::zero());
    gl::clear_buffer_color(2, color::zero());
    gl::clear_buffer_depth(1.0f);
}

void jkgm::renderer_screen_msaa::end_opaque_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::read, screen_gbuffer.fbo);

    gl::bind_framebuffer(gl::framebuffer_bind_target::draw, gbuffer_color_resolve.fbo);
    gl::read_buffer(gl::framebuffer_attachment::color0);
    gl::blit_framebuffer(screen_gbuffer.viewport,
                         gbuffer_color_resolve.viewport,
                         {gl::framebuffer_blit_buffer::color},
                         gl::framebuffer_blit_filter::nearest);

    gl::bind_framebuffer(gl::framebuffer_bind_target::draw, gbuffer_emissive_resolve.fbo);
    gl::read_buffer(gl::framebuffer_attachment::color1);
    gl::blit_framebuffer(screen_gbuffer.viewport,
                         gbuffer_emissive_resolve.viewport,
                         {gl::framebuffer_blit_buffer::color},
                         gl::framebuffer_blit_filter::nearest);

    gl::bind_framebuffer(gl::framebuffer_bind_target::draw, gbuffer_depth_nrm_resolve.fbo);
    gl::read_buffer(gl::framebuffer_attachment::color2);
    gl::blit_framebuffer(screen_gbuffer.viewport,
                         gbuffer_depth_nrm_resolve.viewport,
                         {gl::framebuffer_blit_buffer::color},
                         gl::framebuffer_blit_filter::nearest);
}

jkgm::gl::texture_view jkgm::renderer_screen_msaa::get_resolved_color_texture()
{
    return gbuffer_color_resolve.tex;
}

jkgm::gl::texture_view jkgm::renderer_screen_msaa::get_resolved_emissive_texture()
{
    return gbuffer_emissive_resolve.tex;
}

jkgm::gl::texture_view jkgm::renderer_screen_msaa::get_resolved_depth_normal_texture()
{
    return gbuffer_depth_nrm_resolve.tex;
}

void jkgm::renderer_screen_msaa::begin_compose_opaque_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
    gl::clear_buffer_color(0, color::zero());
}

void jkgm::renderer_screen_msaa::end_compose_opaque_pass() {}

void jkgm::renderer_screen_msaa::begin_transparency_pass()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, screen_buffer.fbo);
}

void jkgm::renderer_screen_msaa::end_transparency_pass() {}

void jkgm::renderer_screen_msaa::end_frame()
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::read, screen_buffer.fbo);
    gl::bind_framebuffer(gl::framebuffer_bind_target::draw, buffer_resolve.fbo);
    gl::blit_framebuffer(screen_buffer.viewport,
                         buffer_resolve.viewport,
                         {gl::framebuffer_blit_buffer::color},
                         gl::framebuffer_blit_filter::nearest);
}

jkgm::gl::texture_view jkgm::renderer_screen_msaa::get_resolved_final_texture()
{
    return buffer_resolve.tex;
}