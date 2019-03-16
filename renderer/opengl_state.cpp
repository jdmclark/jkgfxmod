#include "opengl_state.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"

jkgm::gl::shader jkgm::compile_shader_from_file(fs::path const &filename, gl::shader_type type)
{
    gl::shader rv(type);

    auto fs = make_file_input_stream(filename);
    memory_block mb;
    memory_output_block mob(&mb);
    fs->copy_to(&mob);

    gl::shader_source(rv, make_string_span(""), make_span(mb.str()));
    gl::compile_shader(rv);
    if(!gl::get_shader_compile_status(rv)) {
        LOG_ERROR(
            "Failed to compile ", filename.generic_string(), ": ", gl::get_shader_info_log(rv));
    }

    return rv;
}

void jkgm::link_program_from_files(std::string const &name,
                                   gl::program *prog,
                                   fs::path const &vx,
                                   fs::path const &fg)
{
    auto vx_shader = compile_shader_from_file(vx, gl::shader_type::vertex);
    auto fg_shader = compile_shader_from_file(fg, gl::shader_type::fragment);

    gl::attach_shader(*prog, vx_shader);
    gl::attach_shader(*prog, fg_shader);

    gl::link_program(*prog);

    if(!gl::get_program_link_status(*prog)) {
        LOG_ERROR("Failed to link program ", name, ": ", gl::get_program_info_log(*prog));
    }
}

jkgm::post_model::post_model()
{
    gl::bind_vertex_array(vao);

    std::array<point<2, float>, 4> post_points{make_point(-1.0f, -1.0f),
                                               make_point(1.0f, -1.0f),
                                               make_point(-1.0f, 1.0f),
                                               make_point(1.0f, 1.0f)};
    std::array<uint32_t, 6> post_indices{0, 1, 2, 2, 1, 3};

    gl::enable_vertex_attrib_array(0U);
    gl::bind_buffer(gl::buffer_bind_target::array, vb);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(post_points).as_const_bytes(),
                    gl::buffer_usage::static_draw);
    gl::vertex_attrib_pointer(
        /*index*/ 0,
        /*elements*/ 2,
        gl::vertex_element_type::float32,
        /*normalized*/ false);

    gl::bind_buffer(gl::buffer_bind_target::element_array, ib);
    gl::buffer_data(gl::buffer_bind_target::element_array,
                    make_span(post_indices).as_const_bytes(),
                    gl::buffer_usage::static_draw);
}

jkgm::render_buffer::render_buffer(size<2, int> dims, int num_samples)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_renderbuffer(rbo);
    gl::renderbuffer_storage_multisample(num_samples, gl::renderbuffer_format::depth, dims);

    // Set up opaque framebuffer:
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo);

    gl::bind_texture(gl::texture_bind_target::texture_2d_multisample, tex);
    gl::tex_image_2d_multisample(gl::texture_bind_target::texture_2d_multisample,
                                 num_samples,
                                 gl::texture_internal_format::rgba32f,
                                 dims,
                                 /*fixed sample locations*/ true);
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d_multisample, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

    gl::draw_buffers(gl::draw_buffer::color0);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::post_buffer::post_buffer(size<2, int> dims)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    gl::bind_renderbuffer(rbo);
    gl::renderbuffer_storage(gl::renderbuffer_format::depth, dims);

    gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba32f,
                     dims,
                     gl::texture_pixel_format::rgba,
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

    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color0, tex, 0);

    gl::draw_buffers(gl::draw_buffer::color0);

    auto fbs = gl::check_framebuffer_status(gl::framebuffer_bind_target::any);
    if(gl::check_framebuffer_status(gl::framebuffer_bind_target::any) !=
       gl::framebuffer_status::complete) {
        gl::log_errors();
        LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::hdr_stack_em::hdr_stack_em(size<2, int> dims, int num_passes, float weight)
    : a(dims)
    , b(dims)
    , num_passes(num_passes)
    , weight(weight)
{
}

jkgm::hdr_stack::hdr_stack()
{
    elements.emplace_back(make_size(1024, 1024), /*passes*/ 2, /*weight*/ 1.0f);
    elements.emplace_back(make_size(512, 512), /*passes*/ 4, /*weight*/ 0.5f);
    elements.emplace_back(make_size(256, 256), /*passes*/ 8, /*weight*/ 0.25f);
    elements.emplace_back(make_size(128, 128), /*passes*/ 8, /*weight*/ 0.125f);
}

jkgm::triangle_buffer_model::triangle_buffer_model()
{
    gl::bind_vertex_array(vao);
    gl::enable_vertex_attrib_array(0U);
    gl::bind_buffer(gl::buffer_bind_target::array, pos_buffer);
    gl::buffer_reserve(gl::buffer_bind_target::array,
                       vb_capacity * sizeof(point<4, float>),
                       gl::buffer_usage::stream_draw);
    gl::vertex_attrib_pointer(/*index*/ 0,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);

    gl::enable_vertex_attrib_array(1U);
    gl::bind_buffer(gl::buffer_bind_target::array, texcoord_buffer);
    gl::buffer_reserve(gl::buffer_bind_target::array,
                       vb_capacity * sizeof(point<2, float>),
                       gl::buffer_usage::stream_draw);
    gl::vertex_attrib_pointer(/*index*/ 1,
                              /*elements*/ 2,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);

    gl::enable_vertex_attrib_array(2U);
    gl::bind_buffer(gl::buffer_bind_target::array, color_buffer);
    gl::buffer_reserve(
        gl::buffer_bind_target::array, vb_capacity * sizeof(color), gl::buffer_usage::stream_draw);
    gl::vertex_attrib_pointer(/*index*/ 2,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);
}

void jkgm::triangle_buffer_model::maybe_grow_buffers(unsigned int new_capacity)
{
    if(vb_capacity < new_capacity) {
        vb_capacity = new_capacity;

        pos.resize(vb_capacity, point<4, float>::zero());
        texcoords.resize(vb_capacity, point<2, float>::zero());
        color.resize(vb_capacity, color::zero());

        gl::bind_buffer(gl::buffer_bind_target::array, pos_buffer);
        gl::buffer_reserve(gl::buffer_bind_target::array,
                           vb_capacity * sizeof(point<4, float>),
                           gl::buffer_usage::stream_draw);

        gl::bind_buffer(gl::buffer_bind_target::array, texcoord_buffer);
        gl::buffer_reserve(gl::buffer_bind_target::array,
                           vb_capacity * sizeof(point<2, float>),
                           gl::buffer_usage::stream_draw);

        gl::bind_buffer(gl::buffer_bind_target::array, color_buffer);
        gl::buffer_reserve(gl::buffer_bind_target::array,
                           vb_capacity * sizeof(color),
                           gl::buffer_usage::stream_draw);
    }
}

void jkgm::triangle_buffer_model::update_buffers()
{
    gl::bind_buffer(gl::buffer_bind_target::array, pos_buffer);
    gl::buffer_sub_data(gl::buffer_bind_target::array,
                        /*offset*/ 0U,
                        make_span(pos).subspan(0, num_vertices).as_const_bytes());

    gl::bind_buffer(gl::buffer_bind_target::array, texcoord_buffer);
    gl::buffer_sub_data(gl::buffer_bind_target::array,
                        /*offset*/ 0U,
                        make_span(texcoords).subspan(0, num_vertices).as_const_bytes());

    gl::bind_buffer(gl::buffer_bind_target::array, color_buffer);
    gl::buffer_sub_data(gl::buffer_bind_target::array,
                        /*offset*/ 0U,
                        make_span(color).subspan(0, num_vertices).as_const_bytes());
}

jkgm::srgb_texture::srgb_texture(size<2, int> dims)
    : dims(dims)
{
}

jkgm::opengl_state::opengl_state::opengl_state(size<2, int> screen_res, config const *the_config)
    : screen_renderbuffer(screen_res, the_config->msaa_samples)
    , screen_postbuffer1(screen_res)
    , screen_postbuffer2(screen_res)
{
    LOG_DEBUG("Loading OpenGL assets");

    link_program_from_files(
        "menu", &menu_program, "jkgm/shaders/menu.vert", "jkgm/shaders/menu.frag");
    link_program_from_files(
        "game", &game_program, "jkgm/shaders/game.vert", "jkgm/shaders/game.frag");
    link_program_from_files("post_gauss7",
                            &post_gauss7,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_gauss7.frag");
    link_program_from_files("post_low_pass",
                            &post_low_pass,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_low_pass.frag");
    link_program_from_files("post_to_srgb",
                            &post_to_srgb,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_to_srgb.frag");

    gl::bind_texture(gl::texture_bind_target::texture_2d, menu_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     0,
                     gl::texture_internal_format::srgb_a8,
                     make_size(640, 480),
                     gl::texture_pixel_format::bgra,
                     gl::texture_pixel_type::uint8,
                     make_span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);

    menu_texture_data.resize(640 * 480, color_rgba8::zero());

    gl::bind_texture(gl::texture_bind_target::texture_2d, hud_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     0,
                     gl::texture_internal_format::srgb_a8,
                     make_size(get<x>(screen_res), get<y>(screen_res)),
                     gl::texture_pixel_format::bgra,
                     gl::texture_pixel_type::uint8,
                     make_span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);

    hud_texture_data.resize(volume(screen_res), color_rgba8::zero());
}