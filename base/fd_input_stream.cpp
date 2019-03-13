#include "fd_input_stream.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct helper_impl;

        template <>
        struct helper_impl<platform_family::win32> {
            static size_t read_some(win32::handle_id fd, span<char> dest)
            {
                return win32::read_file(fd, dest);
            }

            static win32::unique_handle duplicate(win32::handle_id fd)
            {
                return win32::duplicate_handle(fd,
                                               /*children inherit*/ false,
                                               {win32::duplicate_flag::same_access});
            }

            static void close(win32::handle_id fd)
            {
                win32::close_handle(fd);
            }
        };

        using helper = helper_impl<build_traits::family>;
    }
}

jkgm::shared_fd_input_stream::shared_fd_input_stream(detail::fd_s_utype fd) noexcept
    : fd(fd)
{
}

size_t jkgm::shared_fd_input_stream::read_some(span<char> dest)
{
    return helper::read_some(fd, dest);
}

jkgm::fd_input_stream::fd_input_stream(detail::fd_s_type &&fd)
    : fd(std::move(fd))
{
}

jkgm::detail::fd_s_utype jkgm::fd_input_stream::get() const
{
    return fd.value();
}

jkgm::detail::fd_s_utype jkgm::fd_input_stream::release()
{
    return fd.release();
}

jkgm::fd_input_stream jkgm::fd_input_stream::duplicate() const
{
    return fd_input_stream(helper::duplicate(*fd));
}

void jkgm::fd_input_stream::close()
{
    helper::close(fd.release());
}

size_t jkgm::fd_input_stream::read_some(span<char> dest)
{
    return helper::read_some(*fd, dest);
}
