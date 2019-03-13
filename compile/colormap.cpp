#include "colormap.hpp"
#include "base/filesystem.hpp"
#include "base/log.hpp"
#include "base/range.hpp"
#include <cstring>

jkgm::colormap_palette::colormap_palette()
{
    data.resize(256, jkgm::color_rgb8::zero());
}

jkgm::colormap_palette::colormap_palette(jkgm::input_stream *is)
{
    data.resize(256, jkgm::color_rgb8::zero());
    is->read(jkgm::make_span(data).as_bytes());
}

jkgm::colormap_light_level::colormap_light_level(jkgm::input_stream *is)
{
    for(auto &row : data) {
        is->read(jkgm::make_span(row).as_bytes());
    }
}

jkgm::colormap_transparency_table::colormap_transparency_table(jkgm::input_stream *is)
{
    for(auto &row : data) {
        is->read(jkgm::make_span(row).as_bytes());
    }
}

jkgm::colormap::colormap(jkgm::input_stream *is)
{
    is->read(jkgm::make_span(&magic, 1).as_bytes());

    if(strncmp(magic.data(), "CMP ", 4) != 0) {
        LOG_ERROR("file is not a valid colormap");
        throw std::runtime_error("invalid colormap");
    }

    is->read(jkgm::make_span(&version, 1).as_bytes());

    if(version != 0x1E) {
        LOG_ERROR("colormap has unknown version ", version);
        throw std::runtime_error("invalid colormap");
    }

    uint32_t trns_bits;
    is->read(jkgm::make_span(&trns_bits, 1).as_bytes());
    transparency = (trns_bits != 0);

    is->read(jkgm::make_span(&tint, 1).as_bytes());

    // Read padding/unknowns
    is->read(jkgm::make_span(unknown).as_bytes());

    // Read palette table. 24-bit RGB.
    palette = std::make_unique<colormap_palette>(is);

    // Read light levels
    light_levels = std::make_unique<colormap_light_level>(is);

    if(transparency != 0) {
        transparency_tables = std::make_unique<colormap_transparency_table>(is);
    }
}

jkgm::color_rgb8 jkgm::colormap::get_direct_color(int index) const
{
    return palette->data.at(index);
}

jkgm::color_rgb8 jkgm::colormap::get_color(int index, int level) const
{
    return palette->data.at(light_levels->data.at(level).at(index));
}

jkgm::color_rgb8 jkgm::colormap::get_color(int index) const
{
    return get_color(index, 63);
}

jkgm::color_rgb8 jkgm::colormap::get_light_color(int index) const
{
    return get_color(index, 0);
}
