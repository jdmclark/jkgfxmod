#pragma once

#include "cstring_view.hpp"
#include "tagged.hpp"
#include <optional>
#include <unordered_map>

namespace jkgm {
    using environment_map = std::unordered_map<std::string, std::string>;
    MAKE_TAGGED_RANGE_TYPE(environment, environment_map, environment_map::value_type);

    using environment_delta_map = std::unordered_map<std::string, std::optional<std::string>>;
    MAKE_TAGGED_RANGE_TYPE(environment_delta,
                           environment_delta_map,
                           environment_delta_map::value_type);

    bool has_environment_variable(cstring_view key);
    std::optional<std::string> get_environment_variable(cstring_view key);
    void set_environment_variable(cstring_view key, std::optional<cstring_view> value);

    environment get_environment();
    environment patch_environment(environment env, environment_delta const &delta);
}
