#pragma once

#include "output_stream.hpp"

namespace jkgm {

    class output_block : public output_stream {
    public:
        virtual void set_position(size_t offset) = 0;
        virtual size_t position() = 0;
    };
}
