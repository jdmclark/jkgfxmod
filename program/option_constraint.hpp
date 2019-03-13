#pragma once

namespace jkgm {

    class options;

    class option_constraint {
    public:
        virtual ~option_constraint();

        virtual void check_constraint(options const &opts) const = 0;
    };

}
