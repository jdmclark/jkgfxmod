#pragma once

#include "abstract_option.hpp"
#include "base/lexical_cast.hpp"
#include "base/log.hpp"
#include <stdexcept>

namespace jkgm {
    template <typename ValueT, typename BindingT, typename DefaultT = ValueT>
    class value_option : public abstract_option {
    private:
        BindingT &binding;
        DefaultT default_value;
        bool is_set = false;

    public:
        value_option(std::string const &name, BindingT &binding, DefaultT const &default_value)
            : abstract_option(name)
            , binding(binding)
            , default_value(default_value)
        {
            return;
        }

        virtual void reset() override
        {
            binding = default_value;
            is_set = false;
        }

        virtual void load_from_arg_list(abstract_argument_queue &arg_list) override
        {
            if(is_set) {
                // Switch is already set, and is being re-set. Raise a warning.
                LOG_WARNING("Option ", name, " is already set");
            }

            if(arg_list.empty()) {
                // Option requires a value
                LOG_ERROR("Option ", name, " requires a value");
                throw std::runtime_error("Invalid arguments");
            }

            is_set = true;
            std::string value = arg_list.peek();
            arg_list.pop();

            try {
                binding = jkgm::lexical_cast<ValueT>(value);
            }
            catch(std::exception const &e) {
                LOG_ERROR("Could not parse value for option ", name, ": ", e.what());
                throw;
            }
        }

        virtual bool has_value() const override
        {
            return is_set;
        }
    };

    template <typename ValueT, typename DefaultT = ValueT>
    std::unique_ptr<abstract_option> make_value_option(std::string const &name,
                                                       ValueT &binding,
                                                       DefaultT const &default_value = ValueT())
    {
        return std::make_unique<value_option<ValueT, ValueT, DefaultT>>(
            name, binding, default_value);
    }

    template <typename ValueT, typename BindingT, typename DefaultT = BindingT>
    std::unique_ptr<abstract_option> make_value_option(std::string const &name,
                                                       BindingT &binding,
                                                       DefaultT const &default_value = BindingT())
    {
        return std::make_unique<value_option<ValueT, BindingT, DefaultT>>(
            name, binding, default_value);
    }
}
