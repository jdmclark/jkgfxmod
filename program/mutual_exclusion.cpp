#include "mutual_exclusion.hpp"
#include "base/log.hpp"
#include "base/oxford_join.hpp"
#include "options.hpp"

jkgm::mutual_exclusion::mutual_exclusion(std::vector<std::string> const &opts,
                                         int min_set,
                                         int max_set)
    : exclusive_options(opts)
    , min_set(min_set)
    , max_set(max_set)
{
    return;
}

void jkgm::mutual_exclusion::check_constraint(options const &opt) const
{
    int count = 0;
    for(auto const &exclusive_option : exclusive_options) {
        if(opt.has_value(exclusive_option)) {
            ++count;
        }
    }

    if(count < min_set) {
        LOG_ERROR("At least ",
                  min_set,
                  " of ",
                  jkgm::oxford_join(exclusive_options),
                  " must be specified");
        throw std::runtime_error("Invalid arguments");
    }
    else if(count > max_set) {
        LOG_ERROR(
            "At most ", max_set, " of ", jkgm::oxford_join(exclusive_options), " may be specified");
        throw std::runtime_error("Invalid arguments");
    }
}
