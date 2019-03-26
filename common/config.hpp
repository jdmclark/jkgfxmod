#pragma once

#include <memory>
#include <string>
#include <tuple>

namespace jkgm {
    class config {
    public:
        std::tuple<int, int> resolution = std::make_tuple(640, 480);
        bool fullscreen = false;
        float max_anisotropy = 2.0f;
        bool enable_ssao = true;
        bool enable_parallax = true;
        std::string command = "jk.exe";
    };

    std::unique_ptr<config> load_config_file();
}