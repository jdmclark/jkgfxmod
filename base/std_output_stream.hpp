#pragma once

#include "output_stream.hpp"

namespace jkgm {
    extern output_stream &std_output;
    extern output_stream &std_error;

    void refresh_std_output();
    void refresh_std_error();
}
