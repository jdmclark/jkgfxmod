#pragma once

#include "base/range.hpp"
#include "options.hpp"

namespace jkgm {
    class program {
    private:
        options opts;

    protected:
        virtual void create_options(options &) = 0;
        virtual int run() = 0;

    public:
        program();
        int start(jkgm::range<char **> const &);
    };
}

#define MAKE_MAIN(ProgramT)                                                                        \
    int main(int argc, char **argv)                                                                \
    {                                                                                              \
        ProgramT program;                                                                          \
        return program.start(jkgm::make_range(argv + 1, argv + argc));                             \
    }
