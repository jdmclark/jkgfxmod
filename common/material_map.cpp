#include "material_map.hpp"
#include "base/file_block.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "base/string_search.hpp"
#include "error_reporter.hpp"
#include "json_incl.hpp"

void jkgm::material_map::add_metadata(fs::path const &metadata_file)
{
    fs::path data_dir = metadata_file.parent_path();

    auto fs = make_file_input_block(metadata_file);
    memory_block mb;
    memory_output_block mob(&mb);
    fs->copy_to(&mob);

    auto doc = json::json::parse(mb.str());

    LOG_DEBUG("Mapping materials from ",
              static_cast<std::string>(doc["name"]),
              " (",
              metadata_file.generic_string(),
              ")");

    for(auto const &em : doc["materials"]) {
        auto mat = std::make_unique<material>();

        if(em.contains("albedo_map")) {
            mat->albedo_map = data_dir / std::string(em["albedo_map"]);
        }

        if(em.contains("albedo_factor")) {
            auto const &af = em["albedo_factor"];
            mat->albedo_factor = color(static_cast<float>(af[0]),
                                       static_cast<float>(af[1]),
                                       static_cast<float>(af[2]),
                                       static_cast<float>(af[3]));
        }

        if(em.contains("emissive_map")) {
            mat->emissive_map = data_dir / std::string(em["emissive_map"]);
        }

        if(em.contains("emissive_factor")) {
            auto const &af = em["emissive_factor"];
            mat->emissive_factor = color_rgb(
                static_cast<float>(af[0]), static_cast<float>(af[1]), static_cast<float>(af[2]));
        }

        if(em.contains("displacement_map")) {
            mat->displacement_map = data_dir / std::string(em["displacement_map"]);
        }

        if(em.contains("displacement_factor")) {
            mat->displacement_factor = em["displacement_factor"];
        }

        if(em.contains("alpha_mode")) {
            auto const &am = em["alpha_mode"];
            if(am == "mask") {
                mat->alpha_mode = material_alpha_mode::mask;
            }
            else if(am == "blend") {
                mat->alpha_mode = material_alpha_mode::blend;
            }
        }

        if(em.contains("alpha_cutoff")) {
            mat->alpha_cutoff = em["alpha_cutoff"];
        }

        auto mat_index = materials.size();
        materials.push_back(std::move(mat));

        for(auto const &sig : em["replaces_signatures"]) {
            signature_map.emplace(md5(static_cast<std::string>(sig)), mat_index);
        }
    }
}

void jkgm::material_map::create_map(fs::path const &materials_dir)
{
    LOG_DEBUG("Loading material map...");
    // Search for metadata.json files in subdirectories
    for(fs::directory_iterator dir_iter(materials_dir); dir_iter != fs::directory_iterator();
        ++dir_iter) {
        if(fs::is_directory(dir_iter->status())) {
            auto md_path = dir_iter->path() / "metadata.json";
            try {
                add_metadata(md_path);
            }
            catch(std::exception const &e) {
                LOG_WARNING(
                    "Failed to load materials from ", md_path.generic_string(), ": ", e.what());
                report_warning_message(
                    str(fmt("JkGfxMod could not load the material pack \"",
                            dir_iter->path().filename().generic_string(),
                            "\". The directory either does not contain a material pack, or the "
                            "material pack is corrupt.\n\nDetails: ",
                            e.what())));
            }
        }
    }

    LOG_DEBUG("Material map loaded");
}

std::optional<jkgm::material const *> jkgm::material_map::get_material(md5 const &sig) const
{
    auto it = signature_map.find(sig);
    if(it != signature_map.end()) {
        return materials.at(it->second).get();
    }

    return std::nullopt;
}

std::vector<std::unique_ptr<jkgm::material>> const &jkgm::material_map::get_materials() const
{
    return materials;
}