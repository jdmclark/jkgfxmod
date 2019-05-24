#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tuple>

namespace jkgm {
    struct antialiasing_config {
        std::string type;
        int samples = 2;
        float min_sample_factor = 0.0f;
    };

    class config {
    public:
        std::tuple<int, int> resolution = std::make_tuple(640, 480);
        std::optional<std::tuple<int, int>> internal_resolution;
        bool fullscreen = false;
        float hud_scale = 1.0f;
        float max_anisotropy = 2.0f;
        std::optional<antialiasing_config> antialiasing;
        bool enable_bloom = true;
        bool enable_ssao = true;
        bool enable_parallax = true;
        bool enable_texture_filtering = true;
        bool enable_posterized_lighting = false;
        std::string command = "jk.exe";
        std::string data_path = "jkgm";
        std::optional<std::string> log_path;
    };

    std::unique_ptr<config> load_config_file();
}