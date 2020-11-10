#include "shaders.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "common/error_reporter.hpp"

namespace jkgm {
    namespace {
        gl::shader compile_shader_from_file(fs::path const &filename, gl::shader_type type)
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
                report_fatal_message(
                    str(format("JkGfxMod could not open an essential file and cannot "
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
                    "JkGfxMod failed to compile an essential shader. This may have happened "
                    "because your "
                    "graphics device does not support OpenGL 3.3, because the shader file has been "
                    "incorrectly modified, or because of a bug in JkGfxMod.\n\nDetails: Failed to "
                    "compile ",
                    filename.generic_string(),
                    "\n",
                    err_str)));
            }

            return rv;
        }

        void link_program_from_files(std::string const &name,
                                     gl::program_view prog,
                                     fs::path const &vx,
                                     fs::path const &fg)
        {
            auto vx_shader = compile_shader_from_file(vx, gl::shader_type::vertex);
            auto fg_shader = compile_shader_from_file(fg, gl::shader_type::fragment);

            gl::attach_shader(prog, vx_shader);
            gl::attach_shader(prog, fg_shader);

            gl::link_program(prog);

            if(!gl::get_program_link_status(prog)) {
                auto err_str = gl::get_program_info_log(prog);
                report_fatal_message(str(format("JkGfxMod failed to link an essential shader. This "
                                                "may have happened because your "
                                                "graphics device does not support OpenGL 3.3, "
                                                "because the shader files have been "
                                                "incorrectly modified, or because of a bug in "
                                                "JkGfxMod.\n\nDetails: Failed to link ",
                                                name,
                                                "\n",
                                                err_str)));
            }
        }
    }
}

jkgm::opengl_shader::opengl_shader(std::string const &name,
                                   fs::path const &vertex_part,
                                   fs::path const &fragment_part)
{
    link_program_from_files(name, the_program, vertex_part, fragment_part);
}

void jkgm::opengl_shader::use_program()
{
    gl::use_program(the_program);
}

jkgm::shaders::menu_shader::menu_shader(fs::path const &data_root)
    : opengl_shader("menu", data_root / "shaders/game.vert", data_root / "shaders/menu.frag")
    , tex(gl::get_uniform_location(the_program, "tex"))
{
}

void jkgm::shaders::menu_shader::set_menu_sampler(int sampler)
{
    gl::set_uniform_integer(tex, sampler);
}

jkgm::shaders::game_pass_shader::game_pass_shader(std::string const &name,
                                                  fs::path const &vertex_part,
                                                  fs::path const &fragment_part)
    : opengl_shader(name, vertex_part, fragment_part)
    , screen_resolution(gl::get_uniform_location(the_program, "screen_resolution"))
    , features(gl::get_uniform_location(the_program, "features"))
    , albedo_map(gl::get_uniform_location(the_program, "albedo_map"))
    , albedo_factor(gl::get_uniform_location(the_program, "albedo_factor"))
    , emissive_map(gl::get_uniform_location(the_program, "emissive_map"))
    , emissive_factor(gl::get_uniform_location(the_program, "emissive_factor"))
    , alpha_cutoff(gl::get_uniform_location(the_program, "alpha_cutoff"))
    , displacement_map(gl::get_uniform_location(the_program, "displacement_map"))
    , displacement_factor(gl::get_uniform_location(the_program, "displacement_factor"))
{
}

jkgm::shaders::game_opaque_pass_shader::game_opaque_pass_shader(fs::path const &data_root)
    : game_pass_shader("game_opaque_pass",
                       data_root / "shaders/game.vert",
                       data_root / "shaders/game_opaque_pass.frag")
{
}

void jkgm::shaders::game_pass_shader::set_screen_resolution(size<2, float> res)
{
    gl::set_uniform_vector(screen_resolution, res);
}

void jkgm::shaders::game_pass_shader::set_features(point<4, float> value)
{
    gl::set_uniform_vector(features, value);
}

void jkgm::shaders::game_pass_shader::set_albedo_map_sampler(int sampler)
{
    gl::set_uniform_integer(albedo_map, sampler);
}

void jkgm::shaders::game_pass_shader::set_albedo_factor(color factor)
{
    gl::set_uniform_vector(albedo_factor, factor);
}

void jkgm::shaders::game_pass_shader::set_emissive_map_sampler(int sampler)
{
    gl::set_uniform_integer(emissive_map, sampler);
}

void jkgm::shaders::game_pass_shader::set_emissive_factor(color_rgb factor)
{
    gl::set_uniform_vector(emissive_factor, factor);
}

void jkgm::shaders::game_pass_shader::set_alpha_cutoff(float cutoff)
{
    gl::set_uniform_float(alpha_cutoff, cutoff);
}

void jkgm::shaders::game_pass_shader::set_displacement_map_sampler(int sampler)
{
    gl::set_uniform_integer(displacement_map, sampler);
}

void jkgm::shaders::game_pass_shader::set_displacement_factor(float factor)
{
    gl::set_uniform_float(displacement_factor, factor);
}

jkgm::shaders::game_post_ssao_shader::game_post_ssao_shader(fs::path const &data_root)
    : opengl_shader("game_post_ssao",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_ssao.frag")
    , normal_image(gl::get_uniform_location(the_program, "normal_image"))
    , depth_image(gl::get_uniform_location(the_program, "depth_image"))
    , noise_image(gl::get_uniform_location(the_program, "noise_image"))
{
    samples.reserve(num_kernel_samples);

    std::string name_buf;
    for(int i = 0; i < num_kernel_samples; ++i) {
        name_buf = str(format("samples[", i, "]"));
        samples.push_back(gl::get_uniform_location(the_program, name_buf));
    }
}

void jkgm::shaders::game_post_ssao_shader::set_normal_sampler(int sampler)
{
    gl::set_uniform_integer(normal_image, sampler);
}

void jkgm::shaders::game_post_ssao_shader::set_depth_sampler(int sampler)
{
    gl::set_uniform_integer(depth_image, sampler);
}

void jkgm::shaders::game_post_ssao_shader::set_noise_sampler(int sampler)
{
    gl::set_uniform_integer(noise_image, sampler);
}

void jkgm::shaders::game_post_ssao_shader::set_samples(span<point<3, float> const> values)
{
    size_t i = 0;
    for(auto const &em : values) {
        gl::set_uniform_vector(samples.at(i++), em);
    }
}

jkgm::shaders::game_post_opaque_composite_shader::game_post_opaque_composite_shader(
    fs::path const &data_root)
    : opengl_shader("game_post_opaque_composite",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_opaque_composite.frag")
    , color_image(gl::get_uniform_location(the_program, "color_image"))
    , emissive_image(gl::get_uniform_location(the_program, "emissive_image"))
    , occlusion_image(gl::get_uniform_location(the_program, "occlusion_image"))
{
}

void jkgm::shaders::game_post_opaque_composite_shader::set_color_sampler(int sampler)
{
    gl::set_uniform_integer(color_image, sampler);
}

void jkgm::shaders::game_post_opaque_composite_shader::set_emissive_sampler(int sampler)
{
    gl::set_uniform_integer(emissive_image, sampler);
}

void jkgm::shaders::game_post_opaque_composite_shader::set_occlusion_sampler(int sampler)
{
    gl::set_uniform_integer(occlusion_image, sampler);
}

jkgm::shaders::game_transparency_pass_shader::game_transparency_pass_shader(
    fs::path const &data_root)
    : game_pass_shader("game_transparency_pass",
                       data_root / "shaders/game.vert",
                       data_root / "shaders/game_trns_pass.frag")
{
}

jkgm::shaders::post_box4_shader::post_box4_shader(fs::path const &data_root)
    : opengl_shader("post_box4",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_box4.frag")
    , fbuf_image(gl::get_uniform_location(the_program, "fbuf_image"))
    , normal_image(gl::get_uniform_location(the_program, "normal_image"))
    , depth_image(gl::get_uniform_location(the_program, "depth_image"))
{
}

void jkgm::shaders::post_box4_shader::set_fbuf_sampler(int sampler)
{
    gl::set_uniform_integer(fbuf_image, sampler);
}

void jkgm::shaders::post_box4_shader::set_normal_sampler(int sampler)
{
    gl::set_uniform_integer(normal_image, sampler);
}

void jkgm::shaders::post_box4_shader::set_depth_sampler(int sampler)
{
    gl::set_uniform_integer(depth_image, sampler);
}

jkgm::shaders::post_gauss7_shader::post_gauss7_shader(fs::path const &data_root)
    : opengl_shader("post_gauss7",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_gauss7.frag")
    , fbuf_image(gl::get_uniform_location(the_program, "fbuf_image"))
    , fbuf_dimensions(gl::get_uniform_location(the_program, "fbuf_dimensions"))
    , blur_direction(gl::get_uniform_location(the_program, "blur_direction"))
{
}

void jkgm::shaders::post_gauss7_shader::set_fbuf_sampler(int sampler)
{
    gl::set_uniform_integer(fbuf_image, sampler);
}

void jkgm::shaders::post_gauss7_shader::set_fbuf_dimensions(size<2, float> dims)
{
    gl::set_uniform_vector(fbuf_dimensions, dims);
}

void jkgm::shaders::post_gauss7_shader::set_blur_direction(direction<2, float> dir)
{
    gl::set_uniform_vector(blur_direction, dir);
}

jkgm::shaders::post_low_pass_shader::post_low_pass_shader(fs::path const &data_root)
    : opengl_shader("post_low_pass",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_low_pass.frag")
    , fbuf_image(gl::get_uniform_location(the_program, "fbuf_image"))
{
}

void jkgm::shaders::post_low_pass_shader::set_fbuf_sampler(int sampler)
{
    gl::set_uniform_integer(fbuf_image, sampler);
}

jkgm::shaders::post_to_srgb_shader::post_to_srgb_shader(fs::path const &data_root)
    : opengl_shader("post_to_srgb",
                    data_root / "shaders/postprocess.vert",
                    data_root / "shaders/post_to_srgb.frag")
    , fbuf_image(gl::get_uniform_location(the_program, "fbuf_image"))
{
    bloom_fbuf.reserve(num_bloom_layers);
    bloom_weight.reserve(num_bloom_layers);

    std::string name_buf;
    for(int i = 0; i < num_bloom_layers; ++i) {
        name_buf = str(format("bloom_fbuf[", i, "]"));
        bloom_fbuf.push_back(gl::get_uniform_location(the_program, name_buf));

        name_buf = str(format("bloom_weight[", i, "]"));
        bloom_weight.push_back(gl::get_uniform_location(the_program, name_buf));
    }
}

void jkgm::shaders::post_to_srgb_shader::set_fbuf_sampler(int sampler)
{
    gl::set_uniform_integer(fbuf_image, sampler);
}

void jkgm::shaders::post_to_srgb_shader::set_bloom_samplers(span<int const> samplers)
{
    size_t i = 0;
    for(auto const &em : samplers) {
        gl::set_uniform_integer(bloom_fbuf.at(i++), em);
    }
}

void jkgm::shaders::post_to_srgb_shader::set_bloom_weights(span<float const> weights)
{
    size_t i = 0;
    for(auto const &em : weights) {
        gl::set_uniform_float(bloom_weight.at(i++), em);
    }
}