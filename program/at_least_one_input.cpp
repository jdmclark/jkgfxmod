#include "at_least_one_input.hpp"
#include "base/log.hpp"
#include "options.hpp"

void jkgm::at_least_one_input::check_constraint(options const &opts) const
{
    if(!opts.has_bare_value()) {
        LOG_ERROR("No input files specified");
        throw std::runtime_error("Invalid arguments");
    }
}
