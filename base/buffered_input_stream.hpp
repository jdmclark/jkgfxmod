#pragma once

#include "constants.hpp"
#include "input_stream.hpp"
#include <array>

namespace jkgm {
    template <size_t N>
    class basic_buffered_input_stream : public input_stream {
        static_assert(N != 0, "buffer size cannot be zero");

    private:
        input_stream *underlying_stream;
        std::array<char, N> buffer;
        span<char const> read_span;

    public:
        explicit basic_buffered_input_stream(input_stream *underlying_stream) noexcept
            : underlying_stream(underlying_stream)
            , read_span(buffer.data(), 0)
        {
        }

        size_t read_some(span<char> dest) override
        {
            if(read_span.empty()) {
                // Refill buffer
                size_t amount_buffered = underlying_stream->read_some(make_span(buffer));
                if(amount_buffered == 0) {
                    // Reached end of stream
                    return 0;
                }

                read_span = make_span(buffer.data(), amount_buffered);
            }

            size_t amount_read = std::min(dest.size(), read_span.size());

            auto read_subspan = read_span.subspan(0, amount_read);
            std::copy(read_subspan.begin(), read_subspan.end(), dest.begin());

            read_span = read_span.subspan(amount_read, read_span.size() - amount_read);
            return amount_read;
        }
    };

    using buffered_input_stream = basic_buffered_input_stream<io_buffer_size>;
}
