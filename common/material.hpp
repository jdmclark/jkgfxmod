#pragma once

#include "base/filesystem.hpp"
#include "math/color.hpp"
#include <optional>

namespace jkgm {
    enum class material_alpha_mode { opaque, mask, blend };

    class material {
    public:
        std::optional<fs::path> albedo_map;
        color albedo_factor = color::fill(1.0f);

        std::optional<fs::path> emissive_map;
        color_rgb emissive_factor = color_rgb::zero();

        material_alpha_mode alpha_mode = material_alpha_mode::opaque;
        float alpha_cutoff = 0.5f;
    };
}