#include "renderer_compositor_bloom.hpp"
#include "math/colors.hpp"
#include "opengl_state.hpp"

void jkgm::renderer_compositor_bloom::end_frame(opengl_state *ogs,
                                                size<2, int> screen_size,
                                                gl::texture_view composed_screen)
{
    // Compose renderbuffer onto window:
    gl::bind_vertex_array(ogs->postmdl.vao);

    // Render low pass for bloom
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, ogs->screen_postbuffer2.fbo);
    gl::set_viewport(make_box(make_point(0, 0), screen_size));

    gl::set_clear_color(solid(colors::black));
    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

    ogs->post_low_pass.use_program();
    ogs->post_low_pass.set_fbuf_sampler(0);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, composed_screen);

    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

    // Blur and down sample:
    gl::set_active_texture_unit(0);

    gl::texture_view src_tx = ogs->screen_postbuffer2.tex;

    ogs->post_gauss7.use_program();
    ogs->post_gauss7.set_fbuf_sampler(0);

    auto hdr_vp_size = static_cast<size<2, float>>(screen_size);
    float hdr_aspect_ratio = get<x>(hdr_vp_size) / get<y>(hdr_vp_size);

    for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
        auto layer_vp_size = static_cast<size<2, float>>(hdr_stack_em.a.viewport.size());
        ogs->post_gauss7.set_fbuf_dimensions(
            make_size(get<x>(layer_vp_size) * hdr_aspect_ratio, get<y>(layer_vp_size)));

        for(int i = 0; i < hdr_stack_em.num_passes; ++i) {
            // Blur horizontally
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.b.fbo);
            gl::set_viewport(hdr_stack_em.b.viewport);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            ogs->post_gauss7.set_blur_direction(make_direction(1.0f, 0.0f));
            gl::bind_texture(gl::texture_bind_target::texture_2d, src_tx);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // Blur vertically
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.a.fbo);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            ogs->post_gauss7.set_blur_direction(make_direction(0.0f, 1.0f));
            gl::bind_texture(gl::texture_bind_target::texture_2d, hdr_stack_em.b.tex);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // Set up next stage
            src_tx = hdr_stack_em.a.tex;
        }
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
    gl::set_viewport(make_box(make_point(0, 0), screen_size));

    gl::set_clear_color(solid(colors::black));
    gl::clear({gl::clear_flag::color, gl::clear_flag::depth});
    gl::disable(gl::capability::depth_test);
    gl::disable(gl::capability::cull_face);

    // Copy to front buffer while converting to srgb
    ogs->post_to_srgb.use_program();
    ogs->post_to_srgb.set_fbuf_sampler(0);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, composed_screen);

    std::array<int, shaders::post_to_srgb_shader::num_bloom_layers> bloom_samplers;
    std::array<float, shaders::post_to_srgb_shader::num_bloom_layers> bloom_weights;

    int curr_sampler = 1;
    for(int i = 0; i < shaders::post_to_srgb_shader::num_bloom_layers; ++i) {
        auto const &hdr_stack_em = ogs->bloom_layers.elements.at(i);

        bloom_samplers.at(i) = curr_sampler;
        gl::set_active_texture_unit(curr_sampler);
        gl::bind_texture(gl::texture_bind_target::texture_2d, hdr_stack_em.a.tex);
        ++curr_sampler;

        bloom_weights.at(i) = hdr_stack_em.weight;
    }

    ogs->post_to_srgb.set_bloom_samplers(make_span(bloom_samplers));
    ogs->post_to_srgb.set_bloom_weights(make_span(bloom_weights));

    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
}