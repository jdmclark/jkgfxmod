#pragma once

#include "glutil/texture.hpp"
#include "math/size.hpp"
#include <memory>

namespace jkgm {
    class config;
    struct opengl_state;

    class renderer_compositor {
    public:
        virtual ~renderer_compositor() = default;

        virtual void end_frame(opengl_state *ogs,
                               size<2, int> screen_size,
                               gl::texture_view composed_screen) = 0;
    };

    std::unique_ptr<renderer_compositor> create_renderer_compositor(config const *the_config);
}
