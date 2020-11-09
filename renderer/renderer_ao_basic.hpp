#pragma once

#include "renderer_ao.hpp"

namespace jkgm {
    class renderer_ao_basic : public renderer_ao {
    public:
        void update_ssao_texture(opengl_state *ogs,
                                 gl::texture_view normal_tex,
                                 gl::texture_view depth_tex) override;
        gl::texture_view get_ssao_texture() const override;
    };
}