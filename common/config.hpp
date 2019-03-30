#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tuple>

namespace jkgm {
    class config {
    public:
        std::tuple<int, int> resolution = std::make_tuple(640, 480);
        std::optional<std::tuple<int, int>> internal_resolution;
        bool fullscreen = false;
        float hud_scale = 1.0f;
        float max_anisotropy = 2.0f;
        bool enable_ssao = true;
        bool enable_parallax = true;
        std::string command = "jk.exe";
    };

    std::unique_ptr<config> load_config_file();
}