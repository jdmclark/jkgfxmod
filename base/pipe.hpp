#pragma once

#include "fd_output_stream.hpp"
#include "filesystem.hpp"
#include <tuple>

namespace jkgm {
    fd_output_stream make_output_file_append_pipe(fs::path const &p);
}
