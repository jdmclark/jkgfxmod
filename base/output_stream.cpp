#include "output_stream.hpp"

void jkgm::output_stream::write(span<char const> src)
{
    size_t amount_remaining = src.size();
    while(amount_remaining > 0) {
        size_t amount_written = write_some(src);
        amount_remaining -= amount_written;
        if(amount_remaining > 0) {
            src = src.subspan(amount_written, amount_remaining);
        }
    }
}
