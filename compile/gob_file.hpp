#pragma once

#include "base/filesystem.hpp"
#include "base/input_block.hpp"
#include "base/input_stream.hpp"
#include <string>

namespace jkgm {
    class gob_file : public jkgm::input_stream {
    private:
        std::unique_ptr<jkgm::input_block> file;
        size_t const chunk_end;

    public:
        gob_file(fs::path const &container_filename, size_t chunk_offset, size_t chunk_length);

        virtual size_t read_some(jkgm::span<char> dest) override;
    };
}
