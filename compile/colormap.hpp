#pragma once

#include "base/input_stream.hpp"
#include "math/color.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace jkgm {
    class colormap_palette {
    public:
        std::vector<jkgm::color_rgb8> data;

        colormap_palette();
        explicit colormap_palette(jkgm::input_stream *is);
    };

    class colormap_light_level {
    public:
        std::array<std::array<uint8_t, 256>, 64> data;

        colormap_light_level() = default;
        explicit colormap_light_level(jkgm::input_stream *is);
    };

    class colormap_transparency_table {
    public:
        std::array<std::array<uint8_t, 256>, 256> data;

        colormap_transparency_table() = default;
        explicit colormap_transparency_table(jkgm::input_stream *is);
    };

    class colormap {
    public:
        std::array<char, 4> magic;
        uint32_t version;
        bool transparency;

        jkgm::color_rgb tint = jkgm::color_rgb::zero();

        // These values are always 0
        std::array<uint32_t, 10> unknown;

        std::unique_ptr<colormap_palette> palette;
        std::unique_ptr<colormap_light_level> light_levels;
        std::unique_ptr<colormap_transparency_table> transparency_tables;

        colormap() = default;
        explicit colormap(jkgm::input_stream *is);

        jkgm::color_rgb8 get_direct_color(int color_index) const;

        jkgm::color_rgb8 get_color(int color_index, int light_level) const;

        jkgm::color_rgb8 get_color(int color_index) const;
        jkgm::color_rgb8 get_light_color(int color_index) const;
    };
}
