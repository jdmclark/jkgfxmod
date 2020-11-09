#pragma once

#include "glutil/framebuffer.hpp"
#include "glutil/renderbuffer.hpp"
#include "glutil/texture.hpp"
#include "renderer_screen.hpp"

namespace jkgm {
    class render_depthbuffer_msaa {
    public:
        gl::renderbuffer rbo;

        box<2, int> viewport;

        render_depthbuffer_msaa(size<2, int> dims, int num_samples);
    };

    class render_buffer_msaa {
    public:
        gl::framebuffer fbo;
        gl::texture tex;

        box<2, int> viewport;

        render_buffer_msaa(size<2, int> dims, int num_samples, render_depthbuffer_msaa *rbo);
    };

    class render_gbuffer_msaa {
    public:
        gl::framebuffer fbo;
        gl::texture color_tex;
        gl::texture emissive_tex;
        gl::texture normal_tex;
        gl::texture depth_tex;

        box<2, int> viewport;

        render_gbuffer_msaa(size<2, int> dims, int num_samples, render_depthbuffer_msaa *rbo);
    };

    class msaa_resolve_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;

        box<2, int> viewport;

        msaa_resolve_buffer(size<2, int> dims,
                            gl::texture_internal_format tif,
                            gl::texture_pixel_format tpf,
                            gl::texture_pixel_type tpt);
    };

    class renderer_screen_msaa : public renderer_screen {
    private:
        render_depthbuffer_msaa shared_depthbuffer;
        render_buffer_msaa screen_buffer;
        render_gbuffer_msaa screen_gbuffer;

        msaa_resolve_buffer gbuffer_color_resolve;
        msaa_resolve_buffer gbuffer_emissive_resolve;
        msaa_resolve_buffer gbuffer_normal_resolve;
        msaa_resolve_buffer gbuffer_depth_resolve;
        msaa_resolve_buffer buffer_resolve;

    public:
        renderer_screen_msaa(size<2, int> screen_res,
                             int num_samples,
                             std::optional<float> min_sample_factor);

        void begin_frame() override;

        void begin_opaque_pass() override;
        void end_opaque_pass() override;
        gl::texture_view get_resolved_color_texture() override;
        gl::texture_view get_resolved_emissive_texture() override;
        gl::texture_view get_resolved_normal_texture() override;
        gl::texture_view get_resolved_depth_texture() override;

        void begin_compose_opaque_pass() override;
        void end_compose_opaque_pass() override;

        void begin_transparency_pass() override;
        void end_transparency_pass() override;

        void end_frame() override;

        gl::texture_view get_resolved_final_texture() override;
    };
}
