#include "opengl_state.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include <random>

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
                     gl::texture_internal_format::rgba32f,
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
                     gl::texture_internal_format::rgba32f,
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
                     gl::texture_internal_format::rgba32f,
                     dims,
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::float32,
                     span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl ::texture_bind_target::texture_2d, 0U);
    gl::framebuffer_texture(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::color1, emissive_tex, 0);

    // Set up depth normal texture:
    gl::bind_texture(gl::texture_bind_target::texture_2d, depth_nrm_tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba32f,
                     dims,
                     gl::texture_pixel_format::rgba,
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
        LOG_ERROR("Failed to create render framebuffer: ", static_cast<int>(fbs));
    }

    gl::bind_framebuffer(gl::framebuffer_bind_target::any, gl::default_framebuffer);
}

jkgm::ssao_occlusion_buffer::ssao_occlusion_buffer(size<2, int> dims)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    gl::bind_renderbuffer(rbo);
    gl::renderbuffer_storage(gl::renderbuffer_format::depth, dims);

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

    gl::framebuffer_renderbuffer(
        gl::framebuffer_bind_target::any, gl::framebuffer_attachment::depth, rbo);
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
    gl::vertex_attrib_pointer(/*index*/ 0,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);

    gl::enable_vertex_attrib_array(1U);
    gl::bind_buffer(gl::buffer_bind_target::array, texcoord_buffer);
    gl::vertex_attrib_pointer(/*index*/ 1,
                              /*elements*/ 2,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);

    gl::enable_vertex_attrib_array(2U);
    gl::bind_buffer(gl::buffer_bind_target::array, color_buffer);
    gl::vertex_attrib_pointer(/*index*/ 2,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false);

    gl::enable_vertex_attrib_array(3U);
    gl::bind_buffer(gl::buffer_bind_target::array, normal_buffer);
    gl::vertex_attrib_pointer(
        /*index*/ 3, /*elements*/ 3, gl::vertex_element_type::float32, /*normalized*/ false);
}

void jkgm::triangle_buffer_model::maybe_grow_buffers(unsigned int new_capacity)
{
    if(vb_capacity < new_capacity) {
        vb_capacity = new_capacity;

        pos.resize(vb_capacity, point<4, float>::zero());
        texcoords.resize(vb_capacity, point<2, float>::zero());
        color.resize(vb_capacity, color::zero());
        normals.resize(vb_capacity, direction<3, float>::zero());
    }
}

void jkgm::triangle_buffer_model::update_buffers()
{
    gl::bind_buffer(gl::buffer_bind_target::array, pos_buffer);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(pos).as_const_bytes(),
                    gl::buffer_usage::stream_draw);

    gl::bind_buffer(gl::buffer_bind_target::array, texcoord_buffer);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(texcoords).as_const_bytes(),
                    gl::buffer_usage::stream_draw);

    gl::bind_buffer(gl::buffer_bind_target::array, color_buffer);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(color).as_const_bytes(),
                    gl::buffer_usage::stream_draw);

    gl::bind_buffer(gl::buffer_bind_target::array, normal_buffer);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(normals).as_const_bytes(),
                    gl::buffer_usage::stream_draw);
}

jkgm::srgb_texture::srgb_texture(size<2, int> dims)
    : dims(dims)
{
}

jkgm::opengl_state::opengl_state::opengl_state(size<2, int> screen_res, config const *the_config)
    : shared_depthbuffer(screen_res)
    , screen_renderbuffer(screen_res, &shared_depthbuffer)
    , screen_postbuffer1(screen_res)
    , screen_postbuffer2(screen_res)
    , gbuffer(screen_res, &shared_depthbuffer)
{
    LOG_DEBUG("Loading OpenGL assets");

    link_program_from_files(
        "menu", &menu_program, "jkgm/shaders/menu.vert", "jkgm/shaders/menu.frag");

    link_program_from_files("game_opaque_pass",
                            &game_opaque_pass_program,
                            "jkgm/shaders/game.vert",
                            "jkgm/shaders/game_opaque_pass.frag");
    link_program_from_files("game_post_ssao",
                            &game_post_ssao_program,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_ssao.frag");
    link_program_from_files("game_post_opaque_composite",
                            &game_post_opaque_composite_program,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_opaque_composite.frag");

    link_program_from_files("game_transparency_pass",
                            &game_transparency_pass_program,
                            "jkgm/shaders/game.vert",
                            "jkgm/shaders/game_trns_pass.frag");

    link_program_from_files("post_gauss3",
                            &post_gauss3,
                            "jkgm/shaders/postprocess.vert",
                            "jkgm/shaders/post_gauss3.frag");
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

    if(the_config->enable_ssao) {
        ssao_occlusionbuffer = std::make_unique<ssao_occlusion_buffer>(screen_res);

        std::uniform_real_distribution<float> ssao_noise_dist(0.0f, 1.0f);
        std::default_random_engine generator;
        std::vector<point<2, float>> ssao_noise;
        ssao_noise.reserve(16);
        for(size_t i = 0; i < 16; ++i) {
            ssao_noise.push_back(normalize(make_point(ssao_noise_dist(generator) * 2.0f - 1.0f,
                                                      ssao_noise_dist(generator) * 2.0f - 1.0f)));
        }

        ssao_noise_texture = std::make_unique<gl::texture>();
        gl::bind_texture(gl::texture_bind_target::texture_2d, *ssao_noise_texture);
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
}