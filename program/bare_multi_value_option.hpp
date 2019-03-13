#pragma once

#include "abstract_bare_option.hpp"
#include "base/lexical_cast.hpp"
#include "base/log.hpp"
#include <memory>
#include <stdexcept>

namespace jkgm {
    template <typename ElementT, typename BindingItT>
    class bare_multi_value_option : public abstract_bare_option {
    private:
        BindingItT binding;
        bool is_set = false;

    public:
        bare_multi_value_option(BindingItT &binding)
            : binding(binding)
        {
            return;
        }

        virtual void reset() override
        {
            is_set = false;
            return;
        }

        virtual void load_from_arg(std::string const &arg, abstract_argument_queue &) override
        {
            try {
                *binding = jkgm::lexical_cast<ElementT>(arg);
            }
            catch(std::exception const &e) {
                LOG_ERROR("Could not parse value for input: ", e.what());
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
    std::unique_ptr<abstract_bare_option> make_bare_multi_value_option(OutputItT output_iterator)
    {
        return std::make_unique<bare_multi_value_option<ElementT, OutputItT>>(output_iterator);
    }
}
