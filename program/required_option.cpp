#include "required_option.hpp"
#include "base/log.hpp"
#include "base/oxford_join.hpp"
#include "options.hpp"

jkgm::required_option::required_option(std::string const &name)
{
    names.push_back(name);
    return;
}

jkgm::required_option::required_option(std::vector<std::string> const &names)
    : names(names)
{
    return;
}

void jkgm::required_option::check_constraint(options const &opt) const
{
    bool all_specified = true;
    for(auto const &name : names) {
        all_specified = all_specified && opt.has_value(name);
    }

    if(!all_specified) {
        LOG_ERROR(((names.size() > 1) ? "Options" : "Option"),
                  " ",
                  jkgm::oxford_join(names),
                  " must be specified");
        throw std::runtime_error("Invalid arguments");
    }
}
