#include "fd_output_stream.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct helper_impl;

        template <>
        struct helper_impl<platform_family::win32> {
            static size_t write_some(win32::handle_id fd, span<char const> src)
            {
                return win32::write_file(fd, src);
            }

            static win32::unique_handle duplicate(win32::handle_id fd)
            {
                return win32::duplicate_handle(
                    fd, /*children inherit*/ false, {win32::duplicate_flag::same_access});
            }

            static void close(win32::handle_id fd)
            {
                win32::close_handle(fd);
            }
        };

        using helper = helper_impl<build_traits::family>;
    }
}

jkgm::shared_fd_output_stream::shared_fd_output_stream(detail::fd_os_utype fd)
    : fd(fd)
{
}

size_t jkgm::shared_fd_output_stream::write_some(span<char const> src)
{
    return helper::write_some(fd, src);
}

jkgm::fd_output_stream::fd_output_stream(detail::fd_os_type &&fd)
    : fd(std::move(fd))
{
}

jkgm::detail::fd_os_utype jkgm::fd_output_stream::get() const
{
    return fd.value();
}

jkgm::detail::fd_os_utype jkgm::fd_output_stream::release()
{
    return fd.release();
}

jkgm::fd_output_stream jkgm::fd_output_stream::duplicate() const
{
    return fd_output_stream(helper::duplicate(*fd));
}

void jkgm::fd_output_stream::close()
{
    helper::close(fd.release());
}

size_t jkgm::fd_output_stream::write_some(span<char const> dest)
{
    return helper::write_some(*fd, dest);
}
