#pragma once

#include "flag_set.hpp"
#include <cstdint>
#include <string>

namespace jkgm {
    enum class log_level : uint8_t { error = 0, warning = 1, info = 2, debug = 3, trace = 4 };

    using log_levels = flag_set<log_level>;

    std::string const &to_string(log_level v);
}