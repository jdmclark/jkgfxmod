#pragma once

#include "color.hpp"

namespace jkgm {
    color srgb_to_linear(color input);
    color linear_to_srgb(color input);

    color_rgba8 rgb565_key_to_srgb_a8(uint16_t input, bool transparent);
    color_rgba8 rgb565_to_srgb_a8(uint16_t input);
    color_rgba8 rgba5551_to_srgb_a8(uint16_t input);
}