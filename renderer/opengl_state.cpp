#include "opengl_state.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "common/error_reporter.hpp"
#include <random>

jkgm::gl::shader jkgm::compile_shader_from_file(fs::path const &filename, gl::shader_type type)
{
    diagnostic_context dc(filename.generic_string());
    gl::shader rv(type);

    memory_block mb;

    try {
        auto fs = make_file_input_stream(filename);
        memory_output_block mob(&mb);
        fs->copy_to(&mob);
    }
    catch(std::exception const &e) {
        report_fatal_message(str(format("JkGfxMod could not open an essential file and cannot "
                                        "continue.\n\nDetails: Error while opening ",
                                        filename.generic_string(),
                                        "\n",
                                        e.what())));
    }

    gl::shader_source(rv, make_string_span(""), make_span(mb.str()));
    gl::compile_shader(rv);
    if(!gl::get_shader_compile_status(rv)) {
        auto err_str = gl::get_shader_info_log(rv);
        report_fatal_message(str(format(
            "JkGfxMod failed to compile an essential shader. This may have happened because your "
            "graphics device does not support OpenGL 3.3, because the shader file has been "
            "incorrectly modified, or because of a bug in JkGfxMod.\n\nDetails: Failed to compile ",
            filename.generic_string(),
            "\n",
            err_str)));
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
        auto err_str = gl::get_program_info_log(*prog);
        report_fatal_message(str(format(
            "JkGfxMod failed to link an essential shader. This may have happened because your "
            "graphics device does not support OpenGL 3.3, because the shader files have been "
            "incorrectly modified, or because of a bug in JkGfxMod.\n\nDetails: Failed to link ",
            name,
            "\n",
            err_str)));
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

jkgm::overlay_model::overlay_model(size<2, int> scr_res, box<2, int> actual_menu_area)
{
    auto w = 2.0f / static_cast<float>(get<x>(scr_res));
    auto h = 2.0f / static_cast<float>(get<y>(scr_res));

    float x0 = (get<x>(actual_menu_area.start) * w) - 1.0f;
    float x1 = (get<x>(actual_menu_area.stop) * w) - 1.0f;
    float y0 = (get<y>(actual_menu_area.start) * h) - 1.0f;
    float y1 = (get<y>(actual_menu_area.stop) * h) - 1.0f;

    gl::bind_vertex_array(vao);

    std::array<point<2, float>, 4> post_points{
        make_point(x0, y0), make_point(x1, y0), make_point(x0, y1), make_point(x1, y1)};
    std::array<point<2, float>, 4> post_texcoords{make_point(0.0f, 1.0f),
                                                  make_point(1.0f, 1.0f),
                                                  make_point(0.0f, 0.0f),
                                                  make_point(1.0f, 0.0f)};
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

    gl::enable_vertex_attrib_array(1U);
    gl::bind_buffer(gl::buffer_bind_target::array, tcb);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(post_texcoords).as_const_bytes(),
                    gl::buffer_usage::static_draw);
    gl::vertex_attrib_pointer(
        /*index*/ 1, /*elements*/ 2, gl::vertex_element_type::float32, /*normalized*/ false);

    gl::bind_buffer(gl::buffer_bind_target::element_array, ib);
    gl::buffer_data(gl::buffer_bind_target::element_array,
                    make_span(post_indices).as_const_bytes(),
                    gl::buffer_usage::static_draw);
}

jkgm::hud_model::hud_model(size<2, int> scr_res,
                           size<2, int> int_scr_res,
                           box<2, int> actual_scr_area,
                           float scale)
{
    // The Jedi Knight HUD is divided into the following regions:
    // - Console: 640x64 band in top center of screen
    // - Health: 64x64 at bottom left of screen
    // - Weapon: 64x64 at bottom right of screen
    // - Selection: Middle region between health and weapon, centered
    // - Crosshairs/reticles: the rest of the screen

    // Theoretically there may be overlap between reticles and the rest of the HUD, but it is not
    // convenient to avoid scaling these unintentionally.

    std::vector<point<2, float>> points;
    points.reserve(20);

    std::vector<point<2, float>> texcoords;
    texcoords.reserve(20);

    std::vector<uint32_t> indices;
    indices.reserve(30);

    auto actual_scr_area_f = static_cast<box<2, float>>(actual_scr_area);
    auto actual_scr_size_f = actual_scr_area_f.size();
    auto int_scr_res_f = static_cast<size<2, float>>(int_scr_res);
    auto scr_res_f = static_cast<size<2, float>>(scr_res);
    float scr_scale_x = get<x>(actual_scr_size_f) / get<x>(int_scr_res_f);
    float scr_scale_y = get<y>(actual_scr_size_f) / get<y>(int_scr_res_f);

    auto cast_scr_point_to_ndc = [&](point<2, float> const &pt) {
        // Convert to physical screen coordinates:
        auto adj_pt = make_point(get<x>(pt) * scr_scale_x, get<y>(pt) * scr_scale_y) +
                      static_cast<direction<2, float>>(actual_scr_area.start - make_point(0, 0));

        // Scale screen point into NDC:
        auto w = 2.0f / get<x>(scr_res_f);
        auto h = 2.0f / get<y>(scr_res_f);

        return make_point((get<x>(adj_pt) * w) - 1.0f, (get<y>(adj_pt) * h) - 1.0f);
    };

    auto convert_tc_box = [&](box<2, int> tcs) {
        float x0 = get<x>(tcs.start) / get<x>(int_scr_res_f);
        float x1 = get<x>(tcs.stop) / get<x>(int_scr_res_f);
        float y0 = get<y>(tcs.start) / get<y>(int_scr_res_f);
        float y1 = get<y>(tcs.stop) / get<y>(int_scr_res_f);

        return make_box(make_point(x0, y0), make_point(x1, y1));
    };

    auto add_sprite = [&](box<2, float> const &pos, box<2, int> const &tcx) {
        int base = points.size();

        auto p0 = cast_scr_point_to_ndc(pos.start);
        auto p1 = cast_scr_point_to_ndc(pos.stop);

        points.emplace_back(get<x>(p0), get<y>(p0));
        points.emplace_back(get<x>(p1), get<y>(p0));
        points.emplace_back(get<x>(p0), get<y>(p1));
        points.emplace_back(get<x>(p1), get<y>(p1));

        auto tc = convert_tc_box(tcx);

        texcoords.emplace_back(get<x>(tc.start), get<y>(tc.stop));
        texcoords.emplace_back(get<x>(tc.stop), get<y>(tc.stop));
        texcoords.emplace_back(get<x>(tc.start), get<y>(tc.start));
        texcoords.emplace_back(get<x>(tc.stop), get<y>(tc.start));

        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    };

    // Extract sprites from the origin HUD:
    auto console_tc = make_box(make_point(64, 0), make_size(get<x>(int_scr_res) - 128, 64));
    auto tl_tc = make_box(make_point(0, 0), make_size(64, 64));
    auto tr_tc = make_box(make_point(get<x>(int_scr_res) - 64, 0), make_size(64, 64));

    auto health_tc = make_box(make_point(0, get<y>(int_scr_res) - 64), make_size(64, 64));
    auto ammo_tc =
        make_box(make_point(get<x>(int_scr_res) - 64, get<y>(int_scr_res) - 64), make_size(64, 64));

    auto select_tc = make_box(make_point(64, get<y>(int_scr_res) - 64),
                              make_size(get<x>(int_scr_res) - 128, 64));

    auto rest_tc =
        make_box(make_point(0, 64), make_size(get<x>(int_scr_res), get<y>(int_scr_res) - 128));

    // Add sprites to the output HUD. Output coordinates are relative to the virtual screen:
    add_sprite(make_box(make_point(0.0f, 64.0f),
                        make_size(get<x>(int_scr_res_f), get<y>(int_scr_res_f) - 128.0f)),
               rest_tc);

    auto sel_sz = make_size(get<x>(int_scr_res_f) - 128.0f, 64.0f) * scale;
    auto sel_off = (get<x>(int_scr_res_f) - get<x>(sel_sz)) * 0.5f;
    add_sprite(make_box(make_point(sel_off, 0.0f), sel_sz), select_tc);

    auto con_sz = make_size(get<x>(int_scr_res_f), 64.0f) * scale;
    auto con_off = (get<x>(int_scr_res_f) - get<x>(con_sz)) * 0.5f;
    add_sprite(make_box(make_point(con_off, get<y>(int_scr_res_f) - get<y>(con_sz)), con_sz),
               console_tc);

    add_sprite(make_box(make_point(0.0f, get<y>(int_scr_res_f) - 64.0f * scale),
                        make_size(64.0f, 64.0f) * scale),
               tl_tc);
    add_sprite(make_box(make_point(get<x>(int_scr_res_f) - 64.0f * scale,
                                   get<y>(int_scr_res_f) - 64.0f * scale),
                        make_size(64.0f, 64.0f) * scale),
               tr_tc);

    add_sprite(make_box(make_point(0.0f, 0.0f), make_size(64.0f, 64.0f) * scale), health_tc);
    add_sprite(make_box(make_point(get<x>(int_scr_res_f) - 64.0f * scale, 0.0f),
                        make_size(64.0f, 64.0f) * scale),
               ammo_tc);

    gl::bind_vertex_array(vao);

    gl::enable_vertex_attrib_array(0U);
    gl::bind_buffer(gl::buffer_bind_target::array, vb);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(points).as_const_bytes(),
                    gl::buffer_usage::static_draw);
    gl::vertex_attrib_pointer(
        /*index*/ 0,
        /*elements*/ 2,
        gl::vertex_element_type::float32,
        /*normalized*/ false);

    gl::enable_vertex_attrib_array(1U);
    gl::bind_buffer(gl::buffer_bind_target::array, tcb);
    gl::buffer_data(gl::buffer_bind_target::array,
                    make_span(texcoords).as_const_bytes(),
                    gl::buffer_usage::static_draw);
    gl::vertex_attrib_pointer(
        /*index*/ 1, /*elements*/ 2, gl::vertex_element_type::float32, /*normalized*/ false);

    gl::bind_buffer(gl::buffer_bind_target::element_array, ib);
    gl::buffer_data(gl::buffer_bind_target::element_array,
                    make_span(indices).as_const_bytes(),
                    gl::buffer_usage::static_draw);

    num_indices = indices.size();
}

jkgm::post_buffer::post_buffer(size<2, int> dims)
    : viewport(make_point(0, 0), dims)
{
    gl::bind_framebuffer(gl::framebuffer_bind_target::any, fbo);

    gl::bind_texture(gl::texture_bind_target::texture_2d, tex);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba16f,
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
    : mmio(nullptr, 0U)
{
    gl::bind_vertex_array(vao);

    gl::bind_buffer(gl::buffer_bind_target::array, vbo);
    gl::enable_vertex_attrib_array(0U);
    gl::vertex_attrib_pointer(/*index*/ 0,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false,
                              /*stride*/ sizeof(triangle_buffer_vertex),
                              /*offset*/ offsetof(triangle_buffer_vertex, pos));
    gl::enable_vertex_attrib_array(1U);
    gl::vertex_attrib_pointer(/*index*/ 1,
                              /*elements*/ 2,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false,
                              /*stride*/ sizeof(triangle_buffer_vertex),
                              /*offset*/ offsetof(triangle_buffer_vertex, texcoords));
    gl::enable_vertex_attrib_array(2U);
    gl::vertex_attrib_pointer(/*index*/ 2,
                              /*elements*/ 4,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false,
                              /*stride*/ sizeof(triangle_buffer_vertex),
                              /*offset*/ offsetof(triangle_buffer_vertex, col));
    gl::enable_vertex_attrib_array(3U);
    gl::vertex_attrib_pointer(/*index*/ 3,
                              /*elements*/ 3,
                              gl::vertex_element_type::float32,
                              /*normalized*/ false,
                              /*stride*/ sizeof(triangle_buffer_vertex),
                              /*offset*/ offsetof(triangle_buffer_vertex, normal));
}

void jkgm::triangle_buffer_model::maybe_grow_buffers(unsigned int new_capacity)
{
    gl::bind_buffer(gl::buffer_bind_target::array, vbo);

    if(vb_capacity < new_capacity) {
        vb_capacity = new_capacity * 2;

        gl::bind_buffer(gl::buffer_bind_target::array, vbo);
        gl::buffer_reserve(gl::buffer_bind_target::array,
                           vb_capacity * sizeof(triangle_buffer_vertex),
                           gl::buffer_usage::stream_draw);
    }

    mmio = gl::map_buffer_range<triangle_buffer_vertex>(
        gl::buffer_bind_target::array, 0, vb_capacity, {gl::buffer_access::write});
}

void jkgm::triangle_buffer_model::update_buffers()
{
    gl::bind_buffer(gl::buffer_bind_target::array, vbo);
    gl::unmap_buffer(gl::buffer_bind_target::array);
}

jkgm::triangle_buffer_sequence::triangle_buffer_sequence()
{
    constexpr size_t num_buffers = 3;
    for(size_t i = 0; i < num_buffers; ++i) {
        trimdls.emplace_back();
    }

    it = trimdls.begin();
}

jkgm::triangle_buffer_models *jkgm::triangle_buffer_sequence::get_current()
{
    return &(*it);
}

void jkgm::triangle_buffer_sequence::swap_next()
{
    ++it;
    if(it == trimdls.end()) {
        it = trimdls.begin();
    }
}

jkgm::opengl_state::opengl_state::opengl_state(size<2, int> screen_res,
                                               size<2, int> internal_screen_res,
                                               box<2, int> actual_scr_area,
                                               box<2, int> actual_menu_area,
                                               config const *the_config)
    : menumdl(screen_res, actual_menu_area)
    , hudmdl(screen_res, internal_screen_res, actual_scr_area, the_config->hud_scale)
    , screen_postbuffer1(screen_res)
    , screen_postbuffer2(screen_res)
{
    LOG_DEBUG("Loading OpenGL assets");

    fs::path data_root(the_config->data_path);

    link_program_from_files(
        "menu", &menu_program, data_root / "shaders/menu.vert", data_root / "shaders/menu.frag");

    link_program_from_files("game_opaque_pass",
                            &game_opaque_pass_program,
                            data_root / "shaders/game.vert",
                            data_root / "shaders/game_opaque_pass.frag");
    link_program_from_files("game_post_ssao",
                            &game_post_ssao_program,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_ssao.frag");
    link_program_from_files("game_post_opaque_composite",
                            &game_post_opaque_composite_program,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_opaque_composite.frag");

    link_program_from_files("game_transparency_pass",
                            &game_transparency_pass_program,
                            data_root / "shaders/game.vert",
                            data_root / "shaders/game_trns_pass.frag");

    link_program_from_files("post_box4",
                            &post_box4,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_box4.frag");
    link_program_from_files("post_gauss7",
                            &post_gauss7,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_gauss7.frag");
    link_program_from_files("post_low_pass",
                            &post_low_pass,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_low_pass.frag");
    link_program_from_files("post_to_srgb",
                            &post_to_srgb,
                            data_root / "shaders/postprocess.vert",
                            data_root / "shaders/post_to_srgb.frag");

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
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_edge);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_edge);

    menu_texture_data.resize(640 * 480, color_rgba8::zero());

    gl::bind_texture(gl::texture_bind_target::texture_2d, hud_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     0,
                     gl::texture_internal_format::srgb_a8,
                     make_size(get<x>(internal_screen_res), get<y>(internal_screen_res)),
                     gl::texture_pixel_format::bgra,
                     gl::texture_pixel_type::uint8,
                     make_span<char const>(nullptr, 0U));
    gl::set_texture_max_level(gl::texture_bind_target::texture_2d, 0U);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::s,
                              gl::texture_wrap_mode::clamp_to_edge);
    gl::set_texture_wrap_mode(gl::texture_bind_target::texture_2d,
                              gl::texture_direction::t,
                              gl::texture_wrap_mode::clamp_to_edge);

    hud_texture_data.resize(volume(internal_screen_res), color_rgba8::zero());
}