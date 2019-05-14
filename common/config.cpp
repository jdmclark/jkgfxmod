#include "config.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "error_reporter.hpp"
#include "json_incl.hpp"

std::unique_ptr<jkgm::config> jkgm::load_config_file()
{
    diagnostic_context dc("jkgm.json");
    auto rv = std::make_unique<config>();

    try {
        auto fs = jkgm::make_file_input_stream("jkgm.json");

        memory_block mb;
        memory_output_block mob(&mb);
        fs->copy_to(&mob);

        auto j = json::json::parse(mb.str());
        if(j.contains("resolution")) {
            j.at("resolution").get_to(rv->resolution);
        }

        if(j.contains("internal_resolution")) {
            auto const &em = j["internal_resolution"];
            if(!em.is_null()) {
                rv->internal_resolution = em;
            }
        }

        if(j.contains("fullscreen")) {
            j.at("fullscreen").get_to(rv->fullscreen);
        }

        if(j.contains("hud_scale")) {
            j.at("hud_scale").get_to(rv->hud_scale);
        }

        if(j.contains("max_anisotropy")) {
            j.at("max_anisotropy").get_to(rv->max_anisotropy);
        }

        if(j.contains("enable_ssao")) {
            j.at("enable_ssao").get_to(rv->enable_ssao);
        }

        if(j.contains("enable_parallax")) {
            j.at("enable_parallax").get_to(rv->enable_parallax);
        }

        if(j.contains("command")) {
            j.at("command").get_to(rv->command);
        }

        if(j.contains("data_path")) {
            j.at("data_path").get_to(rv->data_path);
        }
    }
    catch(std::exception const &e) {
        LOG_WARNING("Failed to load configuration file: ", e.what());
        report_warning_message(
            str(format("JkGfxMod could not load the jkgm.json configuration file. This session "
                       "will use the default options.\n\nDetails: ",
                       e.what())));
    }

    return rv;
}