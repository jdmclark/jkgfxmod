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

        if(j.contains("fullscreen")) {
            j.at("fullscreen").get_to(rv->fullscreen);
        }

        if(j.contains("correct_game_aspect_ratio")) {
            j.at("correct_game_aspect_ratio").get_to(rv->correct_game_aspect_ratio);
        }

        if(j.contains("correct_menu_aspect_ratio")) {
            j.at("correct_menu_aspect_ratio").get_to(rv->correct_menu_aspect_ratio);
        }

        if(j.contains("hud_scale")) {
            j.at("hud_scale").get_to(rv->hud_scale);
        }

        if(j.contains("max_anisotropy")) {
            j.at("max_anisotropy").get_to(rv->max_anisotropy);
        }

        if(j.contains("antialiasing")) {
            auto const &em = j["antialiasing"];
            if(!em.is_null()) {
                antialiasing_config aa;
                aa.type = em.at("type").get_to(aa.type);

                if(em.contains("samples")) {
                    em.at("samples").get_to(aa.samples);
                }

                if(em.contains("min_sample_factor")) {
                    em.at("min_sample_factor").get_to(aa.min_sample_factor);
                }

                rv->antialiasing = aa;
            }
        }

        if(j.contains("enable_bloom")) {
            j.at("enable_bloom").get_to(rv->enable_bloom);
        }

        if(j.contains("enable_ssao")) {
            j.at("enable_ssao").get_to(rv->enable_ssao);
        }

        if(j.contains("enable_parallax")) {
            j.at("enable_parallax").get_to(rv->enable_parallax);
        }

        if(j.contains("enable_texture_filtering")) {
            j.at("enable_texture_filtering").get_to(rv->enable_texture_filtering);
        }

        if(j.contains("enable_posterized_lighting")) {
            j.at("enable_posterized_lighting").get_to(rv->enable_posterized_lighting);
        }

        if(j.contains("command")) {
            j.at("command").get_to(rv->command);
        }

        if(j.contains("data_path")) {
            j.at("data_path").get_to(rv->data_path);
        }

        if(j.contains("log_path")) {
            auto const &em = j["log_path"];
            if(!em.is_null()) {
                rv->log_path = em;
            }
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