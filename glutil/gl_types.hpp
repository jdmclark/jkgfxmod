#pragma once

#include "base/tagged.hpp"
#include <cstdint>

namespace jkgm::gl {
    using enum_type = unsigned int;
    using boolean_type = unsigned char;
    using bitfield_type = unsigned int;
    using byte_type = int8_t;
    using short_type = int16_t;
    using int_type = int32_t;
    using ubyte_type = uint8_t;
    using ushort_type = uint16_t;
    using uint_type = uint32_t;
    using size_type = int32_t;

    constexpr boolean_type false_value = boolean_type(0);
    constexpr boolean_type true_value = boolean_type(1);

    inline constexpr boolean_type to_gl_bool(bool value)
    {
        return value ? true_value : false_value;
    }
}
