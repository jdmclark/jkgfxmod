#include "log_level.hpp"
#include <map>
#include <unordered_map>

namespace {
    std::map<jkgm::log_level, std::string> str_log_level_map{{jkgm::log_level::error, "error"},
                                                             {jkgm::log_level::warning, "warning"},
                                                             {jkgm::log_level::info, "info"},
                                                             {jkgm::log_level::debug, "debug"},
                                                             {jkgm::log_level::trace, "trace"}};
}

std::string const &jkgm::to_string(log_level v)
{
    return str_log_level_map.at(v);
}