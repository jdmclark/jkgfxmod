#pragma once

#include "base/filesystem.hpp"
#include "base/input_stream.hpp"
#include <memory>

namespace jkgm {
    class virtual_container;

    class virtual_file {
    public:
        fs::path const name;

        explicit virtual_file(fs::path const &fn);
        virtual ~virtual_file();

        virtual std::unique_ptr<jkgm::input_stream> open() const = 0;
        virtual virtual_container const &get_parent_container() const = 0;
    };
}
