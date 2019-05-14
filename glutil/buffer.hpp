#pragma once

#include "base/span.hpp"
#include "base/unique_handle.hpp"
#include "gl.hpp"
#include <optional>

namespace jkgm::gl {
    struct buffer_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using buffer = unique_handle<buffer_traits>;
    using buffer_view = unique_handle_view<buffer_traits>;

    enum class buffer_bind_target : enum_type { array = 0x8892, element_array = 0x8893 };

    enum class buffer_usage : enum_type {
        static_draw = 0x88E4,
        dynamic_draw = 0x88E8,
        stream_draw = 0x88E0
    };

    enum class buffer_access : bitfield_type {
        read = 1,
        write = 2,
        invalidate_range = 4,
        invalidate_buffer = 8,
        flush_explicit = 16,
        unsynchronized = 32
    };

    void bind_buffer(buffer_bind_target target, buffer_view buf);
    void buffer_reserve(buffer_bind_target target, size_t size, buffer_usage usage);
    void buffer_data(buffer_bind_target target, span<char const> data, buffer_usage usage);
    void buffer_sub_data(buffer_bind_target target, size_t offset, span<char const> data);

    namespace detail {
        void *map_buffer_range(buffer_bind_target target,
                               size_t offset,
                               size_t length,
                               flag_set<buffer_access> access);
    }

    template <class EmT>
    span<EmT> map_buffer_range(buffer_bind_target target,
                               size_t offset,
                               size_t num,
                               flag_set<buffer_access> access)
    {
        void *ptr =
            detail::map_buffer_range(target, offset * sizeof(EmT), num * sizeof(EmT), access);
        return make_span((EmT *)ptr, num);
    }

    void unmap_buffer(buffer_bind_target target);
}
