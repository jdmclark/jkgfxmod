#pragma once

#include "flag_set.hpp"
#include <cstdint>
#include <string>

namespace jkgm {
    enum class log_level : uint8_t { error = 1, warning = 2, info = 4, debug = 8, trace = 16 };

    using log_levels = flag_set<log_level>;

    std::string const &to_string(log_level v);
}