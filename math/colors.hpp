#pragma once

#include "color.hpp"

namespace jkgm::colors {
    static constexpr color transparent = color(0.0f, 0.0f, 0.0f, 0.0f);

    static constexpr color_rgb black = color_rgb(0.0f, 0.0f, 0.0f);
    static constexpr color_rgb white = color_rgb(1.0f, 1.0f, 1.0f);

    static constexpr color_rgb cornflower_blue = color_rgb(100.0f, 149.0f, 237.0f) / 255.0f;
}
