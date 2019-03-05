#pragma once

#include "gl.hpp"
#include "base/unique_handle.hpp"
#include <optional>

namespace jkgm::gl {
    struct vertex_array_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using vertex_array = unique_handle<vertex_array_traits>;
    using vertex_array_view = unique_handle_view<vertex_array_traits>;

    enum class vertex_element_type : enum_type {
        int8 = 0x1400,
        uint8 = 0x1401,
        int16 = 0x1402,
        uint16 = 0x1403,
        int32 = 0x1404,
        uint32 = 0x1405,
        float32 = 0x1406
    };

    enum class element_type : enum_type { triangles = 0x0004 };
    enum class index_type : enum_type { uint8 = 0x1401, uint16 = 0x1403, uint32 = 0x1405 };

    void bind_vertex_array(vertex_array_view vao);
    void enable_vertex_attrib_array(uint_type index);
    void vertex_attrib_pointer(uint_type index,
                               int_type num_components,
                               vertex_element_type type,
                               bool normalized,
                               size_t stride = 0U,
                               ptrdiff_t offset = 0);

    void draw_elements(element_type type,
                       size_t num_indices,
                       index_type itype,
                       ptrdiff_t offset = 0);
}
