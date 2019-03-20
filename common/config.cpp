#include "config.hpp"
#include "base/file_stream.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "json_incl.hpp"

std::unique_ptr<jkgm::config> jkgm::load_config_file()
{
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

        if(j.contains("msaa_samples")) {
            j.at("msaa_samples").get_to(rv->msaa_samples);
        }

        if(j.contains("max_anisotropy")) {
            j.at("max_anisotropy").get_to(rv->max_anisotropy);
        }

        if(j.contains("command")) {
            j.at("command").get_to(rv->command);
        }
    }
    catch(std::exception const &e) {
        LOG_WARNING("Failed to load configuration file: ", e.what());
    }

    return rv;
}