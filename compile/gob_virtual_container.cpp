#include "gob_virtual_container.hpp"
#include "base/file_block.hpp"
#include "base/log.hpp"

namespace {
    struct gob_header {
        char magic[4];
        uint32_t first_index_offset;
        uint32_t unknown;
        uint32_t index_count;
    };

    struct gob_entry {
        uint32_t chunk_offset;
        uint32_t chunk_length;
        char chunk_name[128];
    };
}

jkgm::gob_virtual_container::gob_virtual_container(fs::path const &container_filename)
    : virtual_container(container_filename)
{
    std::string stk_filename = container_filename.generic_string();
    jkgm::diagnostic_context dc(stk_filename.c_str());

    auto file = jkgm::make_file_input_block(container_filename);

    gob_header header;
    file->read(jkgm::make_span(&header, 1).as_bytes());

    if(strncmp(header.magic, "GOB ", 4) != 0) {
        LOG_ERROR("container is not a valid GOB");
        throw std::runtime_error("Invalid GOB file");
    }

    gob_entry entry;
    for(uint32_t i = 0; i < header.index_count; ++i) {
        file->read(jkgm::make_span(&entry, 1).as_bytes());

        // Convert path separators for boost path.
        std::replace_if(
            entry.chunk_name, entry.chunk_name + 128, [](char ch) { return ch == '\\'; }, '/');

        // Convert entry to lowercase.
        std::transform(entry.chunk_name, entry.chunk_name + 128, entry.chunk_name, tolower);

        entry.chunk_name[127] = '\0';

        files.emplace_back(entry.chunk_name, *this, entry.chunk_offset, entry.chunk_length);
    }

    return;
}

size_t jkgm::gob_virtual_container::size() const
{
    return files.size();
}

jkgm::gob_virtual_file const &jkgm::gob_virtual_container::get_file(size_t index) const
{
    return files[index];
}
