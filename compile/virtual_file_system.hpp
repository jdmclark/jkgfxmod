#pragma once

#include "base/filesystem.hpp"
#include "base/input_stream.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace jkgm {
    class virtual_file_system {
    public:
        virtual ~virtual_file_system();

        virtual std::unique_ptr<jkgm::input_stream> open(fs::path const &filename) const = 0;

        virtual std::tuple<fs::path, std::unique_ptr<jkgm::input_stream>>
            find(fs::path const &filename, std::vector<fs::path> const &prefixes) const = 0;
    };
}
