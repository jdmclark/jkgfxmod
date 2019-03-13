#include "gob_file.hpp"
#include "base/file_block.hpp"

jkgm::gob_file::gob_file(fs::path const &container_filename,
                         size_t chunk_offset,
                         size_t chunk_length)
    : file(jkgm::make_file_input_block(container_filename))
    , chunk_end(chunk_offset + chunk_length)
{
    file->set_position(chunk_offset);
}

size_t jkgm::gob_file::read_some(jkgm::span<char> dest)
{
    size_t desired_size = dest.size();

    size_t read_ceil = chunk_end - file->position();
    size_t actual_want = std::min(read_ceil, desired_size);

    return file->read_some(dest.subspan(0, actual_want));
}