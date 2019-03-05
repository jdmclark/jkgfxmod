#pragma once

#include "constants.hpp"
#include "output_stream.hpp"
#include <vector>

namespace jkgm {
    template <size_t N>
    class basic_buffered_output_stream : public output_stream {
        static_assert(N != 0, "buffer size cannot be zero");

    private:
        output_stream *underlying_stream;
        std::vector<char> buffer;

    protected:
        void flush()
        {
            underlying_stream->write(make_span(buffer));
            buffer.clear();
        }

    public:
        explicit basic_buffered_output_stream(output_stream *underlying_stream) noexcept
            : underlying_stream(underlying_stream)
        {
            buffer.reserve(N);
        }

        ~basic_buffered_output_stream() override
        {
            flush();
        }

        basic_buffered_output_stream(basic_buffered_output_stream const &) = delete;
        basic_buffered_output_stream(basic_buffered_output_stream &&) = delete;
        basic_buffered_output_stream &operator=(basic_buffered_output_stream const &) = delete;
        basic_buffered_output_stream &operator=(basic_buffered_output_stream &&) = delete;

        size_t write_some(span<char const> src) override
        {
            size_t max_write_step = std::min(src.size(), N - buffer.size());

            if(max_write_step > 0) {
                // Copy into buffer
                auto src_subspan = src.subspan(0, max_write_step);
                buffer.insert(buffer.end(), src_subspan.begin(), src_subspan.end());
            }

            if(buffer.size() == N) {
                // Buffer is full
                flush();
            }

            return max_write_step;
        }
    };

    using buffered_output_stream = basic_buffered_output_stream<io_buffer_size>;
}
