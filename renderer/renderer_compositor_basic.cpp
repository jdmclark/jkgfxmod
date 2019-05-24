#include "renderer_compositor_basic.hpp"
#include "math/colors.hpp"
#include "opengl_state.hpp"

void jkgm::renderer_compositor_basic::end_frame(opengl_state *ogs,
                                                size<2, int> screen_size,
                                                gl::texture_view composed_screen)
{
    // Compose renderbuffer onto window:
    gl::bind_vertex_array(ogs->postmdl.vao);

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
        gl::bind_texture(gl::texture_bind_target::texture_2d, gl::default_texture);
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