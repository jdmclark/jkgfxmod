#include "file_block.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct helper_impl;

        template <>
        struct helper_impl<platform_family::win32> {
            static auto open_output(fs::path const &filename)
            {
                return win32::create_file(filename,
                                          {win32::file_access_right::write_data,
                                           win32::file_access_right::append_data},
                                          {win32::file_share_mode::exclusive},
                                          /*children inherit*/ false,
                                          win32::creation_disposition::create_always,
                                          {win32::file_attribute::normal});
            }

            static auto open_input(fs::path const &filename)
            {
                return win32::create_file(filename,
                                          {win32::file_access_right::read_data},
                                          {win32::file_share_mode::share_read},
                                          /*children inherit*/ false,
                                          win32::creation_disposition::open_existing,
                                          {win32::file_attribute::normal});
            }

            static size_t write_some(win32::handle_id fd, span<char const> src)
            {
                return win32::write_file(fd, src);
            }

            static void set_position(win32::handle_id fd, size_t offset)
            {
                win32::set_file_pointer(fd, offset, win32::move_method::begin);
            }

            static size_t position(win32::handle_id fd)
            {
                return win32::get_file_pointer(fd);
            }

            static size_t read_some(win32::handle_id fd, span<char> dest)
            {
                return win32::read_file(fd, dest);
            }

            static void seek(win32::handle_id fd, int offset)
            {
                win32::set_file_pointer(fd, offset, win32::move_method::current);
            }

            static size_t size(win32::handle_id fd)
            {
                return win32::get_file_size(fd);
            }
        };

        using helper = helper_impl<build_traits::family>;
    }
}

jkgm::file_output_block::file_output_block(fs::path const &filename)
    : fd(helper::open_output(filename))
{
}

size_t jkgm::file_output_block::write_some(span<char const> src)
{
    return helper::write_some(*fd, src);
}

void jkgm::file_output_block::set_position(size_t offset)
{
    helper::set_position(*fd, offset);
}

size_t jkgm::file_output_block::position()
{
    return helper::position(*fd);
}

jkgm::file_input_block::file_input_block(fs::path const &filename)
    : fd(helper::open_input(filename))
{
}

size_t jkgm::file_input_block::read_some(span<char> dest)
{
    return helper::read_some(*fd, dest);
}

void jkgm::file_input_block::seek(int offset)
{
    helper::seek(*fd, offset);
}

void jkgm::file_input_block::set_position(size_t offset)
{
    helper::set_position(*fd, offset);
}

size_t jkgm::file_input_block::position()
{
    return helper::position(*fd);
}

size_t jkgm::file_input_block::size()
{
    return helper::size(*fd);
}

std::unique_ptr<jkgm::file_output_block> jkgm::make_file_output_block(fs::path const &filename)
{
    return std::make_unique<file_output_block>(filename);
}

std::unique_ptr<jkgm::file_input_block> jkgm::make_file_input_block(fs::path const &filename)
{
    return std::make_unique<file_input_block>(filename);
}
