#pragma once

#include "virtual_file.hpp"

namespace jkgm {
    class gob_virtual_container;

    class gob_virtual_file : public virtual_file {
    public:
        gob_virtual_container const &parent_container;
        size_t chunk_offset;
        size_t chunk_length;

        gob_virtual_file(fs::path const &name,
                         gob_virtual_container const &pc,
                         size_t chunk_offset,
                         size_t chunk_length);

        virtual std::unique_ptr<jkgm::input_stream> open() const override;
        virtual virtual_container const &get_parent_container() const override;
    };
}
