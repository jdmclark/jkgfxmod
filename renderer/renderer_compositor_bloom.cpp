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

    gl::use_program(ogs->post_low_pass);

    gl::set_uniform_integer(gl::uniform_location_id(0), 0);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, composed_screen);

    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

    // Blur and down sample:
    gl::set_active_texture_unit(0);

    gl::texture_view src_tx = ogs->screen_postbuffer2.tex;

    gl::use_program(ogs->post_gauss7);
    gl::set_uniform_integer(gl::uniform_location_id(0), 0);

    auto hdr_vp_size = static_cast<size<2, float>>(screen_size);
    float hdr_aspect_ratio = get<x>(hdr_vp_size) / get<y>(hdr_vp_size);

    for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
        auto layer_vp_size = static_cast<size<2, float>>(hdr_stack_em.a.viewport.size());
        gl::set_uniform_vector(gl::uniform_location_id(1),
                               make_size(get<x>(layer_vp_size) * hdr_aspect_ratio,
                                         get<y>(layer_vp_size)));

        for(int i = 0; i < hdr_stack_em.num_passes; ++i) {
            // Blur horizontally
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.b.fbo);
            gl::set_viewport(hdr_stack_em.b.viewport);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(1.0f, 0.0f));
            gl::bind_texture(gl::texture_bind_target::texture_2d, src_tx);
            gl::draw_elements(
                gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);

            // Blur vertically
            gl::bind_framebuffer(gl::framebuffer_bind_target::any, hdr_stack_em.a.fbo);

            gl::set_clear_color(solid(colors::black));
            gl::clear({gl::clear_flag::color, gl::clear_flag::depth});

            gl::set_uniform_vector(gl::uniform_location_id(2), make_direction(0.0f, 1.0f));
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
    gl::use_program(ogs->post_to_srgb);

    gl::set_uniform_integer(gl::uniform_location_id(0), 0);

    gl::set_active_texture_unit(0);
    gl::bind_texture(gl::texture_bind_target::texture_2d, composed_screen);

    int curr_em = 1;
    for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
        gl::set_uniform_integer(gl::uniform_location_id(curr_em), curr_em);
        gl::set_active_texture_unit(curr_em);
        gl::bind_texture(gl::texture_bind_target::texture_2d, hdr_stack_em.a.tex);
        ++curr_em;
    }

    curr_em = 5;
    for(auto &hdr_stack_em : ogs->bloom_layers.elements) {
        gl::set_uniform_float(gl::uniform_location_id(curr_em), hdr_stack_em.weight);
        ++curr_em;
    }

    gl::draw_elements(
        gl::element_type::triangles, ogs->postmdl.num_indices, gl::index_type::uint32);
}