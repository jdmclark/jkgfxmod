#pragma once

#include "abstract_option.hpp"
#include "base/lexical_cast.hpp"
#include "base/log.hpp"
#include <stdexcept>

namespace jkgm {
    template <typename ElementT, typename BindingItT>
    class multi_value_option : public abstract_option {
    private:
        BindingItT binding;
        bool is_set = false;

    public:
        multi_value_option(std::string const &name, BindingItT &binding)
            : abstract_option(name)
            , binding(binding)
        {
            return;
        }

        virtual void reset() override
        {
            is_set = false;
            return;
        }

        virtual void load_from_arg_list(abstract_argument_queue &arg_list) override
        {
            if(arg_list.empty()) {
                LOG_ERROR("Option ", name, " requires a value");
                throw std::runtime_error("Invalid arguments");
            }

            auto em = arg_list.peek();
            arg_list.pop();

            try {
                *binding = jkgm::lexical_cast<ElementT>(em);
            }
            catch(std::exception const &e) {
                LOG_ERROR("Could not parse value for option ", name, ": ", e.what());
                throw;
            }

            ++binding;
            is_set = true;
        }

        virtual bool has_value() const override
        {
            return is_set;
        }
    };

    template <typename ElementT = std::string, typename OutputItT>
    std::unique_ptr<abstract_option> make_multi_value_option(std::string const &name,
                                                             OutputItT output_iterator)
    {
        return std::make_unique<multi_value_option<ElementT, OutputItT>>(name, output_iterator);
    }
}
