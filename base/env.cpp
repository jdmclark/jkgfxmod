#include "env.hpp"
#include "build_traits.hpp"
#include "win32.hpp"

bool jkgm::has_environment_variable(cstring_view key)
{
    std::optional<std::string> rv;

    if constexpr(build_traits::is_windows) {
        rv = win32::get_environment_variable(key);
    }

    return rv.has_value() && !rv->empty();
}

std::optional<std::string> jkgm::get_environment_variable(cstring_view key)
{
    std::optional<std::string> rv;

    if constexpr(build_traits::is_windows) {
        rv = win32::get_environment_variable(key);
    }

    if(!rv.has_value() || rv->empty()) {
        return std::nullopt;
    }

    return rv;
}

void jkgm::set_environment_variable(cstring_view key, std::optional<cstring_view> value)
{
    if constexpr(build_traits::is_windows) {
        win32::set_environment_variable(key, value);
    }
}

jkgm::environment jkgm::get_environment()
{
    environment rv;
    if constexpr(build_traits::is_windows) {
        *rv = win32::get_environment_strings();
    }

    return rv;
}

jkgm::environment jkgm::patch_environment(environment env, environment_delta const &delta)
{
    environment rv = std::move(env);
    for(auto const &var : *delta) {
        if(var.second.has_value()) {
            rv->insert_or_assign(var.first, *var.second);
        }
        else {
            rv->erase(var.first);
        }
    }

    return rv;
}
