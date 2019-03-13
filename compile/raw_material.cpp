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

json::json jkgm::raw_material_cel_record::json_serialize_object() const
{
    json::json rv = {{"type", type}, {"color", color_index}, {"unknown_color", unknown_color}};

    if(type == 8) {
        rv["unknown_texture"] = unknown_texture;
        rv["texture_index"] = texture_index;
    }

    return rv;
}

jkgm::raw_material_texture_record::raw_material_texture_record(jkgm::input_stream *is)
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
    for(uint32_t i = 0; i < mipmap_count; ++i) {
        image_data.emplace_back();
        auto &data = image_data.back();

        data.resize(next_width * next_height);
        is->read(jkgm::make_span(data).as_bytes());

        next_width >>= 1;
        next_height >>= 1;
    }
}

json::json jkgm::raw_material_texture_record::json_serialize_object(
    std::string const &base_texture_name,
    std::optional<colormap const *> master_colormap,
    size_t transparent_index) const
{
    json::json rv = {{"width", width},
                     {"height", height},
                     {"uses_transparency", uses_transparency},
                     {"unknown", unknown},
                     {"mipmap_count", mipmap_count}};

    uint32_t texture_num = 0;
    int next_width = width;
    int next_height = height;

    for(auto const &data : image_data) {
        jkgm::image img(jkgm::make_size(next_width, next_height));
        size_t idx = 0;

        std::string sig = "none computed";

        if(master_colormap.has_value()) {
            auto const *cmp = master_colormap.value();

            // Compute signature
            std::vector<uint16_t> u16_colors;
            u16_colors.reserve(img.data.size());

            for(size_t idx = 0; idx < data.size(); ++idx) {
                auto cmp_em = data[idx];
                auto cmp_color = cmp->get_color(cmp_em);
                auto conv_color = jkgm::extend(cmp_color, uint8_t(255));

                if(uses_transparency && (cmp_em == transparent_index)) {
                    // jkgm::get<jkgm::a>(conv_color) = uint8_t(0);
                    conv_color = jkgm::color_rgba8::zero();
                }

                img.data.at(idx) = conv_color;

                // Compute value for signature
                if(uses_transparency) {
                    u16_colors.push_back(jkgm::srgb_a8_to_rgba5551(conv_color));
                }
                else {
                    u16_colors.push_back(jkgm::srgb_a8_to_rgb565(conv_color));
                }
            }

            jkgm::md5_hasher mh;

            uint32_t bounded_width = next_width;
            uint32_t bounded_height = next_height;
            mh.add(jkgm::make_span(&bounded_width, 1).as_const_bytes());
            mh.add(jkgm::make_span(&bounded_height, 1).as_const_bytes());
            mh.add(jkgm::make_span(u16_colors).as_const_bytes());

            sig = static_cast<std::string>(mh.finish());
        }
        else {
            for(size_t idx = 0; idx < data.size(); ++idx) {
                auto cmp_em = data[idx];
                img.data.at(idx) = jkgm::color_rgba8(cmp_em, cmp_em, cmp_em, uint8_t(255U));
            }
        }

        std::string map_name = jkgm::str(jkgm::format(base_texture_name, "-", texture_num, ".png"));

        auto img_of = jkgm::make_file_output_block(map_name);
        jkgm::store_image_png(img_of.get(), img);

        rv["mipmaps"].push_back({{"name", map_name}, {"signature", sig}});

        ++texture_num;
        next_width >>= 1;
        next_height >>= 1;
    }

    return rv;
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

    if(bitdepth != 8) {
        LOG_ERROR("bitdepth ", bitdepth, " is not supported");
        throw std::runtime_error("Invalid MAT file");
    }

    is->read(jkgm::make_span(unknown).as_bytes());

    for(uint32_t i = 0; i < record_count; ++i) {
        cel_records.emplace_back(is);
    }

    for(uint32_t i = 0; i < texture_count; ++i) {
        texture_records.emplace_back(is);
    }
}

json::json
    jkgm::raw_material::json_serialize_object(fs::path const &extract_path,
                                              std::optional<colormap const *> master_colormap) const
{
    std::vector<json::json> cels;
    for(auto &em : cel_records) {
        cels.push_back(em.json_serialize_object());
    }

    std::vector<json::json> texrecs;

    int tex_num = 0;
    for(auto &em : texture_records) {
        std::string texture_base_name =
            jkgm::str(jkgm::format(extract_path.generic_string(), "-", tex_num));

        texrecs.push_back(
            em.json_serialize_object(texture_base_name, master_colormap, transparency));
        ++tex_num;
    }

    json::json rv = {{"magic", std::string(magic.data(), 4)},
                     {"version", version},
                     {"type", type},
                     {"record_count", record_count},
                     {"texture_count", texture_count},
                     {"transparency", transparency},
                     {"bitdepth", bitdepth},
                     {"unknown", unknown},
                     {"cels", cels},
                     {"textures", texrecs}};

    return rv;
}
