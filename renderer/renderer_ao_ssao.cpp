#include "renderer_ao_ssao.hpp"
#include "base/log.hpp"
#include "opengl_state.hpp"
#include <random>

jkgm::ssao_occlusion_buffer::ssao_occlusion_buffer(size<2, int> dims)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::r16f,
                     dims,
                     gl::texture_pixel_format::red,
                     gl::texture_pixel_type::float32,
                     span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d, gl::min_filter::linear);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_edge);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_edge);

    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

    gl::draw_buffers(gl::draw_buffer::color0);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create SSAO framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::renderer_ao_ssao::renderer_ao_ssao(size<2, int> screen_res)
    : ssao_occlusionbuffer(screen_res)
    , ssao_occlusionbuffer2(screen_res)
{
    std::uniform_real_distribution<float> ssao_noise_dist(0.0f, 1.0f);
    std::default_random_engine generator;

    ssao_kernel.reserve(16);
    for(size_t i = 0; i < 16; ++i) {
        float scale = (float)i / 16;
        scale *= scale;
        scale = lerp(0.1f, 1.0f, scale);
        ssao_kernel.push_back(normalize(point<3, float>(ssao_noise_dist(generator) * 2.0f - 1.0f,
                                                        ssao_noise_dist(generator) * 2.0f - 1.0f,
                                                        ssao_noise_dist(generator))) *
                              ssao_noise_dist(generator) * scale);
    }

    std::vector<point<2, float>> ssao_noise;
    ssao_noise.reserve(16);
    for(size_t i = 0; i < 16; ++i) {
        ssao_noise.push_back(normalize(make_point(ssao_noise_dist(generator) * 2.0f - 1.0f,
                                                  ssao_noise_dist(generator) * 2.0f - 1.0f)));
    }

    gl::bind_texture(gl::texture_bind_target::texture_2d, ssao_noise_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rg32f,
                     make_size(4, 4),
                     gl::texture_pixel_format::rg,
                     gl::texture_pixel_type::float32,
                     make_span(ssao_noise).as_const_bytes());
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d, gl::min_filter::nearest);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::nearest);
}

void jkgm::renderer_ao_ssao::update_ssao_texture(opengl_state *ogs,
                                                 gl::texture_view normal_tex,
                                                 gl::texture_view depth_tex)
{
    // Compute SSAO:
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, ssao_occlusionbuffer2.fbo);
    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

    ogs->game_post_ssao_program.use_program();
    ogs->game_post_ssao_program.set_normal_sampler(0);
    ogs->game_post_ssao_program.set_depth_sampler(1);
    ogs->game_post_ssao_program.set_noise_sampler(2);
    ogs->game_post_ssao_program.set_samples(make_span(ssao_kernel));

    gl::set_active_texture_unit(2);
    gl::bind_texture(gl::texture_bind_target::texture_2d, ssao_noise_texture);

    gl::set_active_texture_unit(1);
    gl::bind_texture(gl::texture_bind_target::texture_2d, depth_tex);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, normal_tex);

    gl::bind_vertex_array(ogs->postmdl.vao);
    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

    // Blur SSAO:
    ogs->post_box4.use_program();
    ogs->post_box4.set_fbuf_sampler(0);
    ogs->post_box4.set_normal_sampler(1);
    ogs->post_box4.set_depth_sampler(2);

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, ssao_occlusionbuffer.fbo);
    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

    gl::set_active_texture_unit(2);
    gl::bind_texture(gl::texture_bind_target::texture_2d, depth_tex);

    gl::set_active_texture_unit(1);
    gl::bind_texture(gl::texture_bind_target::texture_2d, normal_tex);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, ssao_occlusionbuffer2.tex);

    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
}

jkgm::gl::texture_view jkgm::renderer_ao_ssao::get_ssao_texture() const
{
    return ssao_occlusionbuffer.tex;
}