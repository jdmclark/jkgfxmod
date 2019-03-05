#include "input_stream.hpp"
#include "constants.hpp"
#include "output_stream.hpp"
#include <cstdio>
#include <fcntl.h>
#include <stdexcept>

void jkgm::input_stream::read(span<char> dest)
{
    size_t amount_remaining = dest.size();
    while(amount_remaining > 0) {
        size_t amount_read = read_some(dest);
        if(amount_read == 0) {
            // Reached end of file inside read
            throw std::runtime_error("input_stream::read size exceeds bounds");
        }

        amount_remaining -= amount_read;
        if(amount_remaining > 0) {
            dest = dest.subspan(amount_read, amount_remaining);
        }
    }
}

void jkgm::input_stream::copy_to(output_stream *s)
{
    char buffer[io_buffer_size];
    for(;;) {
        size_t amt = read_some(make_span(buffer));
        if(amt == 0) {
            break;
        }

        s->write(make_span(buffer).subspan(0, amt));
    }
}
