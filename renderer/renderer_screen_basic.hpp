#pragma once

#include "glutil/framebuffer.hpp"
#include "glutil/renderbuffer.hpp"
#include "glutil/texture.hpp"
#include "renderer_screen.hpp"

namespace jkgm {
    class render_depthbuffer {
    public:
        gl::renderbuffer rbo;

        box<2, int> viewport;

        explicit render_depthbuffer(size<2, int> dims);
    };

    class render_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;

        box<2, int> viewport;

        render_buffer(size<2, int> dims, render_depthbuffer *rbo);
    };

    class render_gbuffer {
    public:
        gl::framebuffer fbo;
        gl::texture color_tex;
        gl::texture emissive_tex;
        gl::texture depth_nrm_tex;

        box<2, int> viewport;

        render_gbuffer(size<2, int> dims, render_depthbuffer *rbo);
    };

    class renderer_screen_basic : public renderer_screen {
    private:
        render_depthbuffer shared_depthbuffer;
        render_buffer screen_buffer;
        render_gbuffer screen_gbuffer;

    public:
        explicit renderer_screen_basic(size<2, int> screen_res);

        void begin_frame() override;

        void begin_opaque_pass() override;
        void end_opaque_pass() override;
        gl::texture_view get_resolved_color_texture() override;
        gl::texture_view get_resolved_emissive_texture() override;
        gl::texture_view get_resolved_depth_normal_texture() override;

        void begin_compose_opaque_pass() override;
        void end_compose_opaque_pass() override;

        void begin_transparency_pass() override;
        void end_transparency_pass() override;

        void end_frame() override;

        gl::texture_view get_resolved_final_texture() override;
    };
}
