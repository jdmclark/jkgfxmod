#pragma once

#include "renderer_compositor.hpp"

namespace jkgm {
    class renderer_compositor_bloom : public renderer_compositor {
    public:
        void end_frame(opengl_state *ogs,
                       size<2, int> screen_size,
                       gl::texture_view composed_screen) override;
    };
}
