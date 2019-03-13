#include "options.hpp"

bool jkgm::options::has_value(std::string const &alias) const
{
    auto const &opt = get_option<abstract_option>(alias);
    return opt.has_value();
}

bool jkgm::options::has_bare_value() const
{
    return bare_option && bare_option->has_value();
}

void jkgm::options::add_alias(std::string const &option, std::string const &alias)
{
    auto it = opt_map.find(option);
    if(it == opt_map.end()) {
        LOG_ERROR("Option ", option, " has not been defined");
        throw std::logic_error("Undefined option");
    }

    auto alias_map_res = alias_map.emplace(alias, it->second);
    if(!alias_map_res.second) {
        LOG_ERROR("Option alias ", alias, " has already been defined");
        throw std::logic_error("Alias redefinition");
    }
}

void jkgm::options::insert(std::unique_ptr<abstract_option> &&opt)
{
    // Sanity check name
    if(std::find_if(opt->name.begin(), opt->name.end(), [](char ch) {
           return jkgm::is_space(ch);
       }) != opt->name.end()) {
        LOG_ERROR("Option ", opt->name, " name cannot contain whitespace");
        throw std::logic_error("Option name cannot contain whitespace");
    }

    opts.push_back(std::forward<std::unique_ptr<abstract_option>>(opt));
    abstract_option *reg_opt = opts.back().get();

    auto opt_map_res = opt_map.emplace(reg_opt->name, reg_opt);
    if(!opt_map_res.second) {
        LOG_ERROR("Option ", reg_opt->name, " has already been defined");
        throw std::logic_error("Option redefinition");
    }

    opt_map.emplace(reg_opt->name, reg_opt);

    auto full_name = jkgm::str(jkgm::format("--", reg_opt->name));
    add_alias(reg_opt->name, full_name);
}

void jkgm::options::insert_bare(std::unique_ptr<abstract_bare_option> &&opt)
{
    bare_option = std::forward<std::unique_ptr<abstract_bare_option>>(opt);
}

void jkgm::options::load_from_arg_queue(abstract_argument_queue &arg_list)
{
    // Reset all opts.
    if(bare_option) {
        bare_option->reset();
    }

    for(auto &opt : opts) {
        opt->reset();
    }

    while(!arg_list.empty()) {
        auto arg = arg_list.peek();
        arg_list.pop();

        // Check if it's a bare option
        if(bare_option && !jkgm::begins_with(arg, "-")) {
            bare_option->load_from_arg(arg, arg_list);
            continue;
        }

        auto it = alias_map.find(arg);
        if(it == alias_map.end()) {
            LOG_ERROR("Unrecognized option ", arg);
            throw std::runtime_error("Invalid argument");
        }
        else {
            it->second->load_from_arg_list(arg_list);
        }
    }

    // Evaluate constraints
    for(auto const &constraint : constraints) {
        constraint->check_constraint(*this);
    }
}
