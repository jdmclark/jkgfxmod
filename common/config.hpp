#pragma once

#include <memory>
#include <string>
#include <tuple>

namespace jkgm {
    class config {
    public:
        std::tuple<int, int> resolution = std::make_tuple(640, 480);
        bool fullscreen = false;
        int msaa_samples = 4;
        float max_anisotropy = 2.0f;
        std::string command = "jk.exe";
    };

    std::unique_ptr<config> load_config_file();
}