#pragma once

#include "build_traits.hpp"
#include <cstddef>

namespace jkgm {
    template <platform_family family>
    constexpr size_t platform_io_buffer_size = 4096U;

    template <>
    constexpr size_t platform_io_buffer_size<platform_family::win32> = 131072U;

    constexpr size_t io_buffer_size = platform_io_buffer_size<build_traits::family>;
}
