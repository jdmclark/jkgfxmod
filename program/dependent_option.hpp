#pragma once

#include "option_constraint.hpp"
#include <string>

namespace jkgm {

    class dependent_option : public option_constraint {
    private:
        std::string option;
        std::string dependency;

    public:
        dependent_option(std::string const &name,
                         std::string const &dependency);

        virtual void check_constraint(options const &opt) const override;
    };

}
