#include "raw_material.hpp"
#include "base/file_block.hpp"
#include "base/filesystem.hpp"
#include "base/log.hpp"
#include "base/md5.hpp"
#include "base/range.hpp"
#include "common/image.hpp"
#include "math/color_conv.hpp"
#include <cstring>

jkgm::raw_material_cel_record::raw_material_cel_record(jkgm::input_stream *is)
{
    is->read(jkgm::make_span(&type, 1).as_bytes());
    if(type != 0 && type != 8) {
        LOG_ERROR("unknown cel type ", type);
        throw std::runtime_error("Invalid MAT file");
    }

    is->read(jkgm::make_span(&color_index, 1).as_bytes());
    is->read(jkgm::make_span(unknown_color).as_bytes());

    if(type == 8) {
        // Texture type: load texture elements
        is->read(jkgm::make_span(unknown_texture).as_bytes());
        is->read(jkgm::make_span(&texture_index, 1).as_bytes());
    }
}

jkgm::raw_material_texture_record::raw_material_texture_record(jkgm::input_stream *is,
                                                               uint32_t bitdepth)
{
    is->read(jkgm::make_span(&width, 1).as_bytes());
    is->read(jkgm::make_span(&height, 1).as_bytes());

    uint32_t ut_bytes;
    is->read(jkgm::make_span(&ut_bytes, 1).as_bytes());
    uses_transparency = (ut_bytes != 0);

    is->read(jkgm::make_span(unknown).as_bytes());
    is->read(jkgm::make_span(&mipmap_count, 1).as_bytes());

    uint32_t next_width = width;
    uint32_t next_height = height;
    uint32_t bytes_per_pixel = bitdepth / 8U;
    for(uint32_t i = 0; i < mipmap_count; ++i) {
        image_data.emplace_back();
        auto &data = image_data.back();

        data.resize(next_width * next_height * bytes_per_pixel);
        is->read(jkgm::make_span(data).as_bytes());

        next_width >>= 1;
        next_height >>= 1;
    }
}

jkgm::raw_material::raw_material(jkgm::input_stream *is)
{
    is->read(jkgm::make_span(magic).as_bytes());

    if(strncmp(magic.data(), "MAT ", 4) != 0) {
        LOG_ERROR("file is not a valid material");
        throw std::runtime_error("Invalid MAT file");
    }

    is->read(jkgm::make_span(&version, 1).as_bytes());

    if(version != 0x32) {
        LOG_ERROR("file has unknown version ", version);
        throw std::runtime_error("Invalid MAT file");
    }

    is->read(jkgm::make_span(&type, 1).as_bytes());
    is->read(jkgm::make_span(&record_count, 1).as_bytes());
    is->read(jkgm::make_span(&texture_count, 1).as_bytes());
    is->read(jkgm::make_span(&transparency, 1).as_bytes());
    is->read(jkgm::make_span(&bitdepth, 1).as_bytes());

    if(bitdepth != 8 && bitdepth != 16) {
        LOG_ERROR("bitdepth ", bitdepth, " is not supported");
        throw std::runtime_error("Invalid MAT file");
    }

    is->read(jkgm::make_span(unknown).as_bytes());

    for(uint32_t i = 0; i < record_count; ++i) {
        cel_records.emplace_back(is);
    }

    for(uint32_t i = 0; i < texture_count; ++i) {
        texture_records.emplace_back(is, bitdepth);
    }
}