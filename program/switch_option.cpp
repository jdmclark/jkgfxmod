#include "switch_option.hpp"
#include "base/log.hpp"

jkgm::switch_option::switch_option(std::string const &name, bool &binding)
    : abstract_option(name)
    , binding(binding)
{
    return;
}

void jkgm::switch_option::reset()
{
    binding = false;
}

void jkgm::switch_option::load_from_arg_list(abstract_argument_queue &)
{
    if(binding) {
        // Switch is already set, and is being re-set. Raise a warning.
        LOG_WARNING("Option ", name, " is already set");
    }

    binding = true;
}

bool jkgm::switch_option::has_value() const
{
    return binding;
}

std::unique_ptr<jkgm::abstract_option> jkgm::make_switch_option(std::string const &name,
                                                                bool &binding)
{
    return std::make_unique<switch_option>(name, binding);
}
