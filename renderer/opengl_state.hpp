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

namespace jkgm {
    gl::shader compile_shader_from_file(fs::path const &filename, gl::shader_type type);

    void link_program_from_files(std::string const &name,
                                 gl::program *prog,
                                 fs::path const &vx,
                                 fs::path const &fg);

    class post_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        post_model();
    };

    class render_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        gl::renderbuffer rbo;
        box<2, int> viewport;

        explicit render_buffer(size<2, int> dims, int num_samples);
    };

    class post_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        gl::renderbuffer rbo;
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

    class triangle_buffer_model {
    private:
        gl::buffer pos_buffer;
        gl::buffer texcoord_buffer;
        gl::buffer color_buffer;
        unsigned int vb_capacity = 10U;

    public:
        gl::vertex_array vao;

        std::vector<point<4, float>> pos;
        std::vector<point<2, float>> texcoords;
        std::vector<jkgm::color> color;
        int num_vertices = 0;

        triangle_buffer_model();

        void maybe_grow_buffers(unsigned int new_capacity);
        void update_buffers();
    };

    struct opengl_state {
        gl::program menu_program;
        gl::program game_program;
        gl::program game_untextured_program;

        gl::program post_gauss7;
        gl::program post_low_pass;
        gl::program post_to_srgb;

        post_model postmdl;
        triangle_buffer_model world_trimdl;
        triangle_buffer_model world_transparent_trimdl;
        triangle_buffer_model gun_trimdl;
        triangle_buffer_model gun_transparent_trimdl;

        gl::texture menu_texture;
        std::vector<color_rgba8> menu_texture_data;

        gl::texture hud_texture;
        std::vector<color_rgba8> hud_texture_data;

        render_buffer screen_renderbuffer;
        post_buffer screen_postbuffer1;
        post_buffer screen_postbuffer2;

        hdr_stack bloom_layers;

        opengl_state(size<2, int> screen_res, config const *the_config);
    };
}