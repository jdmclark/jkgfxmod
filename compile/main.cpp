#include "base/file_block.hpp"
#include "base/filesystem.hpp"
#include "base/input_stream.hpp"
#include "base/log.hpp"
#include "base/md5.hpp"
#include "base/memory_block.hpp"
#include "base/std_input_stream.hpp"
#include "colormap.hpp"
#include "common/image.hpp"
#include "common/json_incl.hpp"
#include "gob_virtual_container.hpp"
#include "jk_virtual_file_system.hpp"
#include "math/color.hpp"
#include "math/color_conv.hpp"
#include "program/program.hpp"
#include "raw_material.hpp"
#include <bitset>
#include <set>

namespace jkgm {
    enum class out_material_alpha_mode { blend, mask };

    class out_material_replacement {
    public:
        std::string name;
        int cel;

        out_material_replacement(std::string name, int cel)
            : name(std::move(name))
            , cel(cel)
        {
        }
    };

    class out_material {
    public:
        std::string name;

        std::vector<out_material_replacement> replaces;
        std::vector<md5> replaces_signatures;

        std::optional<std::string> albedo_map;
        std::optional<color> albedo_factor;

        std::optional<std::string> emissive_map;
        std::optional<color_rgb> emissive_factor;

        std::optional<std::string> displacement_map;
        std::optional<float> displacement_factor;

        std::optional<out_material_alpha_mode> alpha_mode;
        std::optional<float> alpha_cutoff;
    };

    class out_material_pack {
    public:
        std::string name;
        std::string description;
        std::optional<std::string> comment;

        std::vector<std::unique_ptr<out_material>> materials;
    };

    class processed_names_map {
    public:
        std::set<std::string> seen_material_names;
        std::map<fs::path, std::string> srgb_map;
        std::map<fs::path, std::string> linear_map;
    };

    bool is_pow2(int dim)
    {
        std::bitset<sizeof(dim) * 8> bs(dim);
        return bs.count() == 1;
    }

    std::string get_convert_linear_image(processed_names_map *map,
                                         fs::path const &in_path,
                                         std::string const &desired_name,
                                         fs::path const &desired_out_path)
    {
        auto it = map->linear_map.find(in_path);
        if(it != map->linear_map.end()) {
            // File has already been processed. Reuse the same file.
            return it->second;
        }

        diagnostic_context dc(in_path.generic_string());

        // File has not been seen before.
        try {
            auto is = make_file_input_block(in_path);
            auto img = load_image(is.get());

            if(!is_pow2(get<x>(img->dimensions)) || !is_pow2(get<y>(img->dimensions))) {
                LOG_ERROR("Image dimensions are not power of two");
                throw std::runtime_error("invalid image");
            }

            auto os = make_file_output_block(desired_out_path);
            store_image_png(os.get(), *img);

            // Add to map
            map->linear_map.emplace(in_path, desired_name);
            return desired_name;
        }
        catch(std::exception const &e) {
            LOG_ERROR("Failed to load image file: ", e.what());
            throw;
        }
    }

    std::string get_convert_image(processed_names_map *map,
                                  fs::path const &in_path,
                                  std::string const &desired_name,
                                  fs::path const &desired_out_path)
    {
        auto it = map->srgb_map.find(in_path);
        if(it != map->srgb_map.end()) {
            // File has already been processed. Reuse the same file.
            return it->second;
        }

        diagnostic_context dc(in_path.generic_string());

        // File has not been seen before.
        try {
            auto is = make_file_input_block(in_path);
            auto img = load_image(is.get());

            if(!is_pow2(get<x>(img->dimensions)) || !is_pow2(get<y>(img->dimensions))) {
                LOG_ERROR("Image dimensions are not power of two");
                throw std::runtime_error("invalid image");
            }

            // Premultiply alpha
            for(auto &em : img->data) {
                auto linear_col = srgb_to_linear(to_float_color(em));
                auto pma_col =
                    extend(get<rgb>(linear_col) * get<a>(linear_col), get<a>(linear_col));
                em = to_discrete_color(linear_to_srgb(pma_col));
            }

            auto os = make_file_output_block(desired_out_path);
            store_image_png(os.get(), *img);

            // Add to map
            map->srgb_map.emplace(in_path, desired_name);
            return desired_name;
        }
        catch(std::exception const &e) {
            LOG_ERROR("Failed to load image file: ", e.what());
            throw;
        }
    }

    std::vector<md5> get_mat_cel_signatures(std::string const &mat_filename,
                                            int celnum,
                                            virtual_file_system *vfs,
                                            std::vector<std::unique_ptr<colormap>> const &colormaps)
    {
        std::vector<md5> rv;

        auto mf = vfs->open(mat_filename);
        raw_material mat(mf.get());

        auto const &cel = mat.cel_records.at(celnum);
        auto const &tex = mat.texture_records.at(cel.texture_index);

        uint32_t next_width = tex.width;
        uint32_t next_height = tex.height;
        for(auto const &data : tex.image_data) {
            for(auto const &cmp : colormaps) {
                md5_hasher mh;
                mh.add(make_span(&next_width, 1).as_const_bytes());
                mh.add(make_span(&next_height, 1).as_const_bytes());

                for(auto const &px : data) {
                    auto cmp_color = cmp->get_direct_color(px);
                    auto conv_color = extend(cmp_color, uint8_t(255));

                    if(tex.uses_transparency && (px == mat.transparency)) {
                        conv_color = color_rgba8::zero();
                    }

                    uint16_t conv_value;
                    if(tex.uses_transparency) {
                        conv_value = srgb_a8_to_rgba5551(conv_color);
                    }
                    else {
                        conv_value = srgb_a8_to_rgb565(conv_color);
                    }

                    mh.add(make_span(&conv_value, 1).as_const_bytes());
                }

                rv.push_back(mh.finish());
            }

            next_width >>= 1;
            next_height >>= 1;
        }

        return rv;
    }

    std::unique_ptr<out_material>
        process_material(json::json const &doc,
                         processed_names_map *img_map,
                         virtual_file_system *vfs,
                         std::vector<std::unique_ptr<colormap>> const &colormaps,
                         fs::path const &script_path,
                         fs::path const &output_path)
    {
        auto rv = std::make_unique<out_material>();

        if(!doc.contains("name")) {
            LOG_ERROR("Material does not define a name");
            throw std::runtime_error("Invalid script");
        }

        rv->name = doc["name"];

        std::string lc_name;
        lc_name.reserve(rv->name.size());
        std::transform(rv->name.begin(), rv->name.end(), std::back_inserter(lc_name), to_lower);
        if(img_map->seen_material_names.find(lc_name) != img_map->seen_material_names.end()) {
            LOG_ERROR("Material name '", rv->name, "' defined multiple times");
            throw std::runtime_error("Invalid script");
        }

        img_map->seen_material_names.insert(std::move(lc_name));

        LOG_INFO(rv->name);

        if(rv->name.find_first_of("<>:\"/\\|?*") != std::string::npos) {
            LOG_ERROR("Material '", rv->name, "' name contains invalid characters");
            throw std::runtime_error("Invalid script");
        }

        if(doc.contains("albedo_map")) {
            auto map_filename = str(format(rv->name, ".albedo.png"));
            auto real_map_filename =
                get_convert_image(img_map,
                                  /*input path*/ script_path.parent_path() /
                                      static_cast<std::string>(doc["albedo_map"]),
                                  /*desired name*/ map_filename,
                                  /*desired out path*/ output_path / map_filename);
            rv->albedo_map = real_map_filename;
        }

        if(doc.contains("albedo_factor")) {
            auto const &af = doc["albedo_factor"];
            if(!af.is_array() || af.size() != 4) {
                LOG_ERROR("Material '", rv->name, "' albedo factor must be an RGBA tuple");
                throw std::runtime_error("Invalid script");
            }

            rv->albedo_factor = color(static_cast<float>(af[0]),
                                      static_cast<float>(af[1]),
                                      static_cast<float>(af[2]),
                                      static_cast<float>(af[3]));
        }

        if(doc.contains("emissive_map")) {
            auto map_filename = str(format(rv->name, ".emissive.png"));
            auto real_map_filename =
                get_convert_image(img_map,
                                  /*input path*/ script_path.parent_path() /
                                      static_cast<std::string>(doc["emissive_map"]),
                                  /*desired name*/ map_filename,
                                  /*desired out path*/ output_path / map_filename);
            rv->emissive_map = real_map_filename;
        }

        if(doc.contains("emissive_factor")) {
            auto const &ef = doc["emissive_factor"];
            if(!ef.is_array() || ef.size() != 3) {
                LOG_ERROR("Material '", rv->name, "' emissive factor must be an RGB tuple");
                throw std::runtime_error("Invalid script");
            }

            rv->emissive_factor = color_rgb(
                static_cast<float>(ef[0]), static_cast<float>(ef[1]), static_cast<float>(ef[2]));
        }

        if(doc.contains("displacement_map")) {
            auto map_filename = str(format(rv->name, ".displacement.png"));
            auto real_map_filename =
                get_convert_linear_image(img_map,
                                         /*input path*/ script_path.parent_path() /
                                             static_cast<std::string>(doc["displacement_map"]),
                                         /*desired name*/ map_filename,
                                         /*desired out path*/ output_path / map_filename);
            rv->displacement_map = real_map_filename;
        }

        if(doc.contains("displacement_factor")) {
            auto const &ef = doc["displacement_factor"];
            if(!ef.is_number()) {
                LOG_ERROR("Material '", rv->name, "' displacement factor must be a number");
                throw std::runtime_error("Invalid script");
            }

            rv->displacement_factor = static_cast<float>(ef);
        }

        if(doc.contains("alpha_mode")) {
            auto const &am = doc["alpha_mode"];
            if(am == "mask") {
                rv->alpha_mode = out_material_alpha_mode::mask;
            }
            else if(am == "blend") {
                rv->alpha_mode = out_material_alpha_mode::blend;
            }
            else {
                LOG_ERROR("Material '",
                          rv->name,
                          "' unknown blend mode '",
                          static_cast<std::string>(am),
                          "'");
                throw std::runtime_error("Invalid script");
            }
        }

        if(doc.contains("alpha_cutoff")) {
            if(rv->alpha_mode != out_material_alpha_mode::mask) {
                LOG_ERROR(
                    "Material '", rv->name, "' specifies alpha cutoff without alpha mode mask");
                throw std::runtime_error("Invalid script");
            }

            rv->alpha_cutoff = doc["alpha_cutoff"];
        }

        // Get signatures for all replaced materials
        std::set<md5> signatures;

        if(!doc.contains("replaces")) {
            LOG_ERROR("Material '", rv->name, "' does not define any replacements");
            throw std::runtime_error("Invalid script");
        }

        for(auto const &em : doc["replaces"]) {
            std::string mat_filename;
            int celnum = 0;

            if(em.is_string()) {
                // Simple replacement: material only, cel 0
                mat_filename = em;
            }
            else if(em.is_array() && em.size() == 2) {
                // Brief replacement: material and cel tuple
                mat_filename = em[0];
                celnum = em[1];
            }
            else {
                LOG_ERROR("Material '", rv->name, "' contains an unreadable replacement");
                throw std::runtime_error("Invalid script");
            }

            rv->replaces.emplace_back(mat_filename, celnum);

            diagnostic_context dc(mat_filename);
            try {
                auto new_sigs = get_mat_cel_signatures(mat_filename, celnum, vfs, colormaps);
                std::copy(new_sigs.begin(),
                          new_sigs.end(),
                          std::inserter(signatures, signatures.begin()));
            }
            catch(std::exception const &e) {
                LOG_ERROR("Failed to load MAT file: ", e.what());
                throw std::runtime_error("Invalid script");
            }
        }

        rv->replaces_signatures.reserve(signatures.size());
        std::copy(
            signatures.begin(), signatures.end(), std::back_inserter(rv->replaces_signatures));

        return rv;
    }

    json::json write_packfile_material(out_material const &mat)
    {
        json::json rv;

        rv["name"] = mat.name;

        for(auto const &repl : mat.replaces) {
            rv["replaces"].push_back({{"name", repl.name}, {"cel", repl.cel}});
        }

        for(auto const &repl : mat.replaces_signatures) {
            rv["replaces_signatures"].push_back(static_cast<std::string>(repl));
        }

        if(mat.albedo_map.has_value()) {
            rv["albedo_map"] = *mat.albedo_map;
        }

        if(mat.albedo_factor.has_value()) {
            auto c = *mat.albedo_factor;
            rv["albedo_factor"] = {get<0>(c), get<1>(c), get<2>(c), get<3>(c)};
        }

        if(mat.emissive_map.has_value()) {
            rv["emissive_map"] = *mat.emissive_map;
        }

        if(mat.emissive_factor.has_value()) {
            auto c = *mat.emissive_factor;
            rv["emissive_factor"] = {get<0>(c), get<1>(c), get<2>(c)};
        }

        if(mat.displacement_map.has_value()) {
            rv["displacement_map"] = *mat.displacement_map;
        }

        if(mat.displacement_factor.has_value()) {
            rv["displacement_factor"] = *mat.displacement_factor;
        }

        if(mat.alpha_mode.has_value()) {
            switch(*mat.alpha_mode) {
            default:
            case out_material_alpha_mode::blend:
                rv["alpha_mode"] = "blend";
                break;

            case out_material_alpha_mode::mask:
                rv["alpha_mode"] = "mask";
                break;
            }
        }

        if(mat.alpha_mode.has_value() && mat.alpha_cutoff.has_value()) {
            rv["alpha_cutoff"] = *mat.alpha_cutoff;
        }

        return rv;
    }

    void write_packfile(fs::path const &output_path, out_material_pack const &pack)
    {
        json::json doc;

        // Add author metadata
        doc["name"] = pack.name;
        doc["description"] = pack.description;

        if(pack.comment.has_value()) {
            doc["comment"] = *pack.comment;
        }

        // Add material mappings
        for(auto const &em : pack.materials) {
            doc["materials"].push_back(write_packfile_material(*em));
        }

        auto out_fs = make_file_output_block(output_path / "metadata.json");
        out_fs->write(make_span(doc.dump(4)));
    }

    class compile_program : public program {
    public:
        fs::path script_path;
        fs::path output_path;

        fs::path jk_resource;
        fs::path jk_episode;
        fs::path jk_game;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("script", script_path));
            opts.insert(make_value_option("output", output_path));

            opts.insert(make_value_option("game", jk_game));
            opts.insert(make_value_option("episode", jk_episode));
            opts.insert(make_value_option("resource", jk_resource));

            opts.emplace_constraint<required_option>(
                std::vector<std::string>{"script", "output", "resource"});
        }

        virtual int run() override
        {
            // Set up virtual file system from command line arguments
            std::unique_ptr<jk_virtual_file_system> vfs;
            if(jk_game.empty()) {
                vfs = std::make_unique<jk_virtual_file_system>(jk_resource);
            }
            else {
                vfs = std::make_unique<jk_virtual_file_system>(jk_resource, jk_game);
            }

            std::unique_ptr<virtual_container> episode_gob;
            if(!jk_episode.empty()) {
                episode_gob = std::make_unique<gob_virtual_container>(jk_episode);
                vfs->set_current_episode(*episode_gob);
            }

            // Load colormaps from virtual file system
            LOG_INFO("Loading colormaps...");
            std::vector<std::string> colormap_names;
            for(auto const &em : vfs->list_files()) {
                if(std::get<0>(em).find(".cmp") != std::string::npos) {
                    colormap_names.push_back(std::get<0>(em));
                }
            }

            std::vector<std::unique_ptr<colormap>> colormaps;
            for(auto const &em : colormap_names) {
                diagnostic_context dc(em);
                try {
                    auto f = vfs->open(em);
                    colormaps.push_back(std::make_unique<colormap>(f.get()));
                }
                catch(std::exception const &e) {
                    LOG_ERROR("Failed to load colormap: ", e.what());
                    return EXIT_FAILURE;
                }
            }

            // Create output directory
            fs::create_directories(output_path);

            // Process script
            memory_block script_mb;

            try {
                auto script_fs = make_file_input_block(script_path);
                memory_output_block mob(&script_mb);
                script_fs->copy_to(&mob);
            }
            catch(std::exception const &e) {
                LOG_ERROR("Failed to load script file: ", e.what());
                return EXIT_FAILURE;
            }

            auto script_doc = json::json::parse(script_mb.str());

            // Assemble pack
            out_material_pack pack;

            if(!script_doc.contains("name")) {
                LOG_ERROR("Script does not contain a name field");
                return EXIT_FAILURE;
            }

            pack.name = script_doc["name"];

            if(!script_doc.contains("description")) {
                LOG_ERROR("Script does not contain a description field");
                return EXIT_FAILURE;
            }

            pack.description = script_doc["description"];

            if(script_doc.contains("comment")) {
                pack.comment = script_doc["comment"];
            }

            if(!script_doc.contains("materials")) {
                LOG_ERROR("Script does not contain a materials field");
                return EXIT_FAILURE;
            }

            LOG_INFO("Compiling materials...");
            processed_names_map img_map;
            for(auto const &em : script_doc["materials"]) {
                try {
                    pack.materials.push_back(process_material(
                        em, &img_map, vfs.get(), colormaps, script_path, output_path));
                }
                catch(std::exception const &e) {
                    LOG_ERROR("Failed to process script: ", e.what());
                    return EXIT_FAILURE;
                }
            }

            LOG_INFO("Done");

            // Output final metadata file
            write_packfile(output_path, pack);

            return EXIT_SUCCESS;
        }
    };
}

MAKE_MAIN(jkgm::compile_program)
