#pragma once

#include "span.hpp"
#include <cstdio>
#include <type_traits>

namespace jkgm {
    class output_stream;

    class input_stream {
    protected:
        input_stream() = default;

    public:
        virtual ~input_stream() = default;

        input_stream(input_stream const &) = default;
        input_stream(input_stream &&) = default;
        input_stream &operator=(input_stream const &) = default;
        input_stream &operator=(input_stream &&) = default;

        void read(span<char> dest);
        virtual size_t read_some(span<char> dest) = 0;

        void copy_to(output_stream *s);
    };
}
