#pragma once

#include "glutil/texture.hpp"
#include <memory>

namespace jkgm {
    class config;
    struct opengl_state;

    class renderer_ao {
    public:
        virtual ~renderer_ao() = default;

        virtual void update_ssao_texture(opengl_state *ogs,
                                         gl::texture_view normal_tex,
                                         gl::texture_view depth_tex) = 0;
        virtual gl::texture_view get_ssao_texture() const = 0;
    };

    std::unique_ptr<renderer_ao> create_renderer_ao(config const *the_config,
                                                    size<2, int> screen_res);
}