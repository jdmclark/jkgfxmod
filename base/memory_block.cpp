#include "memory_block.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

jkgm::memory_input_block::memory_input_block(memory_block const *mf)
    : mf(mf)
{
}

size_t jkgm::memory_input_block::read_some(span<char> dest)
{
    size_t amt = std::min(mf->buffer.size() - offset, dest.size());

    auto first_it = mf->buffer.begin() + offset;
    std::copy(first_it, first_it + amt, dest.begin());

    offset += amt;

    return amt;
}

void jkgm::memory_input_block::seek(int off)
{
    int new_offset = static_cast<int>(offset) + off;

    if(new_offset < 0 || new_offset > static_cast<int>(mf->buffer.size())) {
        throw std::range_error("memory_block::seek invalid offset");
    }

    offset = static_cast<size_t>(new_offset);
}

void jkgm::memory_input_block::set_position(size_t off)
{
    if(off > mf->buffer.size()) {
        throw std::range_error("memory_block::set_position invalid offset");
    }

    offset = off;
}

size_t jkgm::memory_input_block::position()
{
    return offset;
}

size_t jkgm::memory_input_block::size()
{
    return mf->buffer.size();
}

char const *jkgm::memory_block::data() const
{
    return buffer.data();
}

size_t jkgm::memory_block::size() const
{
    return buffer.size();
}

bool jkgm::memory_block::empty() const
{
    return buffer.empty();
}

std::string_view jkgm::memory_block::str() const
{
    return {buffer.data(), buffer.size()};
}

void jkgm::memory_block::str(std::string_view value)
{
    buffer.resize(value.size());
    std::copy(value.begin(), value.end(), buffer.begin());
}

void jkgm::memory_block::clear()
{
    buffer.clear();
}

jkgm::memory_output_block::memory_output_block(memory_block *mf)
    : mf(mf)
{
}

size_t jkgm::memory_output_block::write_some(span<char const> src)
{
    size_t new_end = offset + src.size();
    if(new_end > mf->buffer.size()) {
        mf->buffer.resize(new_end);
    }

    std::copy(src.begin(), src.end(), mf->buffer.begin() + offset);
    offset += src.size();

    return src.size();
}

void jkgm::memory_output_block::set_position(size_t off)
{
    if(off > mf->buffer.size()) {
        throw std::range_error("memory_block::set_position invalid offset");
    }

    offset = off;
}

size_t jkgm::memory_output_block::position()
{
    return offset;
}
