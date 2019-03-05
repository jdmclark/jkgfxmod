#pragma once

#include "span.hpp"
#include <cstdio>
#include <type_traits>

namespace jkgm {
    class output_stream {
    protected:
        output_stream() = default;

    public:
        virtual ~output_stream() = default;

        output_stream(output_stream const &) = default;
        output_stream(output_stream &&) = default;
        output_stream &operator=(output_stream const &) = default;
        output_stream &operator=(output_stream &&) = default;

        void write(span<char const> src);
        virtual size_t write_some(span<char const> src) = 0;
    };
}
