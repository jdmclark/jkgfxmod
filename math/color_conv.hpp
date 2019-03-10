#pragma once

#include "color.hpp"

namespace jkgm {
    color srgb_to_linear(color input);

    color_rgba8 rgb565_key_to_linear(uint16_t input, bool transparent);
    color_rgba8 rgb565_to_linear(uint16_t input);
    color_rgba8 rgba5551_to_linear(uint16_t input);
}