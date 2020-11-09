#pragma once

#include "glutil/framebuffer.hpp"
#include "glutil/texture.hpp"
#include "renderer_ao.hpp"
#include <vector>

namespace jkgm {
    class ssao_occlusion_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        box<2, int> viewport;

        explicit ssao_occlusion_buffer(size<2, int> dims);
    };

    class renderer_ao_ssao : public renderer_ao {
    private:
        ssao_occlusion_buffer ssao_occlusionbuffer;
        ssao_occlusion_buffer ssao_occlusionbuffer2;
        gl::texture ssao_noise_texture;
        std::vector<point<3, float>> ssao_kernel;

    public:
        explicit renderer_ao_ssao(size<2, int> screen_res);

        void update_ssao_texture(opengl_state *ogs,
                                 gl::texture_view normal_tex,
                                 gl::texture_view depth_tex) override;
        gl::texture_view get_ssao_texture() const override;
    };
}