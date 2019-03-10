#pragma once

#include <tuple>
#include <string>
#include <memory>

namespace jkgm {
    class config {
    public:
        std::tuple<int, int> resolution = std::make_tuple(640, 480);
        bool fullscreen = false;
        std::string command = "jk.exe";
    };

    std::unique_ptr<config> load_config_file();
}