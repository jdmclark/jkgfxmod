#include "pipe.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct pipe_maker_impl;

        template <>
        struct pipe_maker_impl<platform_family::win32> {
            static auto make_pipe()
            {
                return win32::create_pipe(/*children inherit handles*/ false);
            }

            static auto make_input_file_pipe(fs::path const &p)
            {
                return win32::create_file(p,
                                          {win32::file_access_right::read_data},
                                          {win32::file_share_mode::share_read},
                                          /*children inherit*/ false,
                                          win32::creation_disposition::open_existing,
                                          {win32::file_attribute::normal});
            }

            static auto make_output_file_pipe(fs::path const &p)
            {
                return win32::create_file(p,
                                          {win32::file_access_right::append_data},
                                          {win32::file_share_mode::share_write},
                                          /*children inherit*/ false,
                                          win32::creation_disposition::create_always,
                                          {win32::file_attribute::normal});
            }

            static auto make_output_file_append_pipe(fs::path const &p)
            {
                return win32::create_file(p,
                                          {win32::file_access_right::append_data},
                                          {win32::file_share_mode::share_write},
                                          /*children inherit*/ false,
                                          win32::creation_disposition::open_always,
                                          {win32::file_attribute::normal});
            }
        };

        using pipe_maker = pipe_maker_impl<build_traits::family>;
    }
}

jkgm::fd_output_stream jkgm::make_output_file_append_pipe(fs::path const &p)
{
    return jkgm::fd_output_stream(pipe_maker::make_output_file_append_pipe(p));
}
