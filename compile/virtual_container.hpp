#pragma once

#include "virtual_container_iterator.hpp"
#include "virtual_file.hpp"

namespace jkgm {

    class virtual_container {
        friend class virtual_container_iterator;

    protected:
        virtual virtual_file const &get_file(size_t index) const = 0;

    public:
        fs::path const container_filename;

        explicit virtual_container(fs::path const &container_filename);
        virtual ~virtual_container();

        virtual size_t size() const = 0;

        virtual_container_iterator begin() const;
        virtual_container_iterator end() const;
    };
}
