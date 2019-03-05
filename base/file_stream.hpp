#pragma once

#include "filesystem.hpp"
#include "input_stream.hpp"
#include "output_stream.hpp"

namespace jkgm {
    std::unique_ptr<output_stream> make_file_output_stream(fs::path const &filename);
    std::unique_ptr<input_stream> make_file_input_stream(fs::path const &filename);
}
