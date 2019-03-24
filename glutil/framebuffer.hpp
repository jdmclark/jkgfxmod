#pragma once

#include "base/span.hpp"
#include "base/unique_handle.hpp"
#include "gl.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"

namespace jkgm::gl {
    struct framebuffer_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using framebuffer = unique_handle<framebuffer_traits>;
    using framebuffer_view = unique_handle_view<framebuffer_traits>;

    enum class draw_buffer : enum_type {
        none = 0,
        front_left = 0x400,
        front_right = 0x401,
        back_left = 0x402,
        back_right = 0x403,
        color0 = 0x8CE0,
        color1 = 0x8CE1,
        color2 = 0x8CE2
    };

    enum class framebuffer_attachment : enum_type {
        color0 = 0x8CE0,
        color1 = 0x8CE1,
        color2 = 0x8CE2,
        depth = 0x8D00,
        stencil = 0x8D20,
        depth_stencil = 0x821A
    };

    enum class framebuffer_bind_target : enum_type { any = 0x8D40, draw = 0x8CA9, read = 0x8CA8 };

    enum class framebuffer_blit_buffer : bitfield_type { color = 0x00004000, depth = 0x00000100 };
    using framebuffer_blit_mask = flag_set<framebuffer_blit_buffer>;

    enum class framebuffer_blit_filter : enum_type { nearest = 0x2600, linear = 0x2601 };

    enum class framebuffer_status : enum_type {
        complete = 0x8CD5,
        undefined = 0x8219,
        incomplete_attachment = 0x8CD6,
        incomplete_missing_attachment = 0x8CD7,
        incomplete_draw_buffer = 0x8CDB,
        incomplete_read_buffer = 0x8CDC,
        unsupported = 0x8CDD,
        incomplete_multisample = 0x8D56,
        incomplete_layer_targets = 0x8DA8
    };

    constexpr framebuffer_view default_framebuffer = framebuffer_view(0U);

    void bind_framebuffer(framebuffer_bind_target target, framebuffer_view buf);

    void blit_framebuffer(box<2, int> src,
                          box<2, int> dest,
                          framebuffer_blit_mask mask,
                          framebuffer_blit_filter filter);

    framebuffer_status check_framebuffer_status(framebuffer_bind_target target);

    namespace detail {
        void draw_buffers_span(span<enum_type const> bufs);
    }

    template <class... T>
    void draw_buffers(T... bufs)
    {
        std::array<enum_type, sizeof...(bufs)> dbs{static_cast<enum_type>(bufs)...};
        detail::draw_buffers_span(make_span(dbs));
    }

    void framebuffer_texture(framebuffer_bind_target target,
                             framebuffer_attachment attachment,
                             texture_view tex,
                             int level);
    void framebuffer_texture_2d(framebuffer_bind_target target,
                                framebuffer_attachment attachment,
                                texture_bind_target textarget,
                                texture_view tex,
                                int level);
    void framebuffer_renderbuffer(framebuffer_bind_target target,
                                  framebuffer_attachment attachment,
                                  renderbuffer_view buf);
}
