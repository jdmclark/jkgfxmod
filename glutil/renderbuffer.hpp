#pragma once

#include "gl.hpp"
#include "base/unique_handle.hpp"

namespace jkgm::gl {
    struct renderbuffer_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using renderbuffer = unique_handle<renderbuffer_traits>;
    using renderbuffer_view = unique_handle_view<renderbuffer_traits>;

    enum class renderbuffer_format { depth = 0x1902, stencil = 0x1901 };

    void bind_renderbuffer(renderbuffer_view buf);
    void renderbuffer_storage(renderbuffer_format fmt, size<2, int> dimensions);
    void renderbuffer_storage_multisample(size_type num_samples,
                                          renderbuffer_format fmt,
                                          size<2, int> dimensions);
}
