#pragma once

#include "option_constraint.hpp"
#include <string>

namespace jkgm {

    class at_least_one_input : public option_constraint {
    public:
        virtual void check_constraint(options const &opts) const;
    };
}
