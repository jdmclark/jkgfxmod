#pragma once

#include "base/filesystem.hpp"
#include "common/config.hpp"
#include "glutil/buffer.hpp"
#include "glutil/framebuffer.hpp"
#include "glutil/program.hpp"
#include "glutil/renderbuffer.hpp"
#include "glutil/shader.hpp"
#include "glutil/texture.hpp"
#include "glutil/vertex_array.hpp"
#include "shaders.hpp"
#include <map>
#include <optional>
#include <vector>

namespace jkgm {
    class post_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        post_model();
    };

    class overlay_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer tcb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        overlay_model(size<2, int> scr_res, box<2, int> actual_menu_area);
    };

    class hud_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer tcb;
        gl::buffer ib;
        unsigned int num_indices = 0U;

        hud_model(size<2, int> scr_res,
                  size<2, int> int_scr_res,
                  box<2, int> actual_scr_area,
                  float scale);
    };

    class post_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        box<2, int> viewport;

        explicit post_buffer(size<2, int> dims);
    };

    class hdr_stack_em {
    public:
        post_buffer a;
        post_buffer b;
        int num_passes;
        float weight;

        hdr_stack_em(size<2, int> dims, int num_passes, float weight);
    };

    class hdr_stack {
    public:
        std::vector<hdr_stack_em> elements;

        hdr_stack();
    };

    struct alignas(32) triangle_buffer_vertex {
        point<4, float> pos;
        point<2, float> texcoords;
        jkgm::color col;
        direction<3, float> normal;
    };

    class triangle_buffer_model {
    private:
        gl::buffer vbo;
        unsigned int vb_capacity = 0U;

    public:
        gl::vertex_array vao;

        span<triangle_buffer_vertex> mmio;
        int num_vertices = 0;

        triangle_buffer_model();

        void maybe_grow_buffers(unsigned int new_capacity);
        void update_buffers();
    };

    struct triangle_buffer_models {
        triangle_buffer_model world_trimdl;
        triangle_buffer_model world_transparent_trimdl;
        triangle_buffer_model gun_trimdl;
        triangle_buffer_model gun_transparent_trimdl;
        triangle_buffer_model overlay_trimdl;
    };

    class triangle_buffer_sequence {
    private:
        std::vector<triangle_buffer_models> trimdls;
        std::vector<triangle_buffer_models>::iterator it;

    public:
        triangle_buffer_sequence();

        triangle_buffer_models *get_current();
        void swap_next();
    };

    struct opengl_state {
        fs::path data_root;

        shaders::menu_shader menu_program;

        shaders::game_opaque_pass_shader game_opaque_pass_program;
        shaders::game_post_ssao_shader game_post_ssao_program;
        shaders::game_post_opaque_composite_shader game_post_opaque_composite_program;

        shaders::game_transparency_pass_shader game_transparency_pass_program;

        shaders::post_box4_shader post_box4;
        shaders::post_gauss7_shader post_gauss7;
        shaders::post_low_pass_shader post_low_pass;
        shaders::post_to_srgb_shader post_to_srgb;

        post_model postmdl;
        overlay_model menumdl;
        hud_model hudmdl;
        triangle_buffer_sequence tribuf;

        gl::texture menu_texture;
        std::vector<color_rgba8> menu_texture_data;

        gl::texture hud_texture_a;
        gl::texture hud_texture_b;
        gl::texture_view hud_texture_front;
        gl::texture_view hud_texture_back;
        std::vector<color_rgba8> hud_texture_data;

        post_buffer screen_postbuffer1;
        post_buffer screen_postbuffer2;

        hdr_stack bloom_layers;

        opengl_state(size<2, int> screen_res,
                     size<2, int> internal_screen_res,
                     box<2, int> actual_screen_area,
                     box<2, int> actual_menu_area,
                     config const *the_config);
    };
}