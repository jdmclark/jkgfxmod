#pragma once

#include "input_stream.hpp"

namespace jkgm {

    class input_block : public input_stream {
    public:
        virtual void seek(int offset) = 0;
        virtual void set_position(size_t offset) = 0;
        virtual size_t position() = 0;

        virtual size_t size() = 0;
    };
}
