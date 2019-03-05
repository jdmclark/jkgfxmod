#pragma once

#include "gl.hpp"
#include "base/span.hpp"
#include "base/unique_handle.hpp"
#include <optional>

namespace jkgm::gl {
    enum class shader_type : enum_type { vertex = 0x8B31, fragment = 0x8B30 };

    struct shader_traits {
        using value_type = uint_type;

        static uint_type create(shader_type type);
        static void destroy(uint_type id);
    };

    using shader = unique_handle<shader_traits>;
    using shader_view = unique_handle_view<shader_traits>;

    void shader_source(shader_view id, span<char const> prefix_text, span<char const> src_text);
    void compile_shader(shader_view id);

    bool get_shader_compile_status(shader_view id);
    std::string get_shader_info_log(shader_view id);
}
