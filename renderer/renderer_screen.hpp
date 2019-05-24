#pragma once

#include "glutil/texture.hpp"
#include "math/size.hpp"
#include <memory>

namespace jkgm {
    class config;

    class renderer_screen {
    public:
        virtual ~renderer_screen() = default;

        virtual void begin_frame() = 0;

        virtual void begin_opaque_pass() = 0;
        virtual void end_opaque_pass() = 0;
        virtual gl::texture_view get_resolved_color_texture() = 0;
        virtual gl::texture_view get_resolved_emissive_texture() = 0;
        virtual gl::texture_view get_resolved_depth_normal_texture() = 0;

        virtual void begin_compose_opaque_pass() = 0;
        virtual void end_compose_opaque_pass() = 0;

        virtual void begin_transparency_pass() = 0;
        virtual void end_transparency_pass() = 0;

        virtual void end_frame() = 0;

        virtual gl::texture_view get_resolved_final_texture() = 0;
    };

    std::unique_ptr<renderer_screen> create_renderer_screen(config const *the_config,
                                                            size<2, int> screen_res);
}