#pragma once

#include "virtual_file_system.hpp"

namespace jkgm {

    class native_file_system : public virtual_file_system {
    private:
        fs::path const base_path;

    public:
        native_file_system() = default;
        explicit native_file_system(fs::path const &base_path);

        virtual std::unique_ptr<jkgm::input_stream> open(fs::path const &filename) const override;
        virtual std::tuple<fs::path, std::unique_ptr<jkgm::input_stream>>
            find(fs::path const &filename, std::vector<fs::path> const &prefixes) const override;
    };
}
