#pragma once

#include "base/filesystem.hpp"
#include "base/input_stream.hpp"
#include "colormap.hpp"
#include "common/json_incl.hpp"
#include "math/color.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace jkgm {
    class raw_material_cel_record {
    public:
        uint32_t type;
        uint32_t color_index;
        std::array<float, 4> unknown_color;
        std::array<uint32_t, 3> unknown_texture;
        uint32_t texture_index;

        explicit raw_material_cel_record(jkgm::input_stream *is);
    };

    class raw_material_texture_record {
    public:
        uint32_t width;
        uint32_t height;
        bool uses_transparency;
        std::array<uint32_t, 2> unknown;
        uint32_t mipmap_count;
        std::vector<std::vector<uint8_t>> image_data;

        explicit raw_material_texture_record(jkgm::input_stream *is);
    };

    class raw_material {
    public:
        std::array<char, 4> magic;
        uint32_t version;

        uint32_t type;
        uint32_t record_count;
        uint32_t texture_count;

        uint32_t transparency;
        uint32_t bitdepth;

        std::array<uint32_t, 12> unknown;

        std::vector<raw_material_cel_record> cel_records;
        std::vector<raw_material_texture_record> texture_records;

        explicit raw_material(jkgm::input_stream *is);
    };
}
