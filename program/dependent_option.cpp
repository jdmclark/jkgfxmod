#include "dependent_option.hpp"
#include "base/log.hpp"
#include "options.hpp"

jkgm::dependent_option::dependent_option(std::string const &option, std::string const &dependency)
    : option(option)
    , dependency(dependency)
{
    return;
}

void jkgm::dependent_option::check_constraint(options const &opt) const
{
    if(opt.has_value(option) && !opt.has_value(dependency)) {
        LOG_ERROR("option ", option, " requires option ", dependency);
        throw std::runtime_error("Invalid arguments");
    }
}
