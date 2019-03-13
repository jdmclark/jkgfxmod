#include "std_output_stream.hpp"
#include "fd_output_stream.hpp"
#include "win32.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct helper_impl;

        template <>
        struct helper_impl<platform_family::win32> {
            static win32::handle_id get_stdout()
            {
                return win32::get_std_handle(win32::std_device::std_output);
            }

            static win32::handle_id get_stderr()
            {
                return win32::get_std_handle(win32::std_device::std_error);
            }
        };

        using helper = helper_impl<build_traits::family>;

        jkgm::shared_fd_output_stream full_std_output(helper::get_stdout());

        jkgm::shared_fd_output_stream full_std_error(helper::get_stderr());
    }
}

jkgm::output_stream &jkgm::std_output = jkgm::full_std_output;
jkgm::output_stream &jkgm::std_error = jkgm::full_std_error;

void jkgm::refresh_std_output()
{
    full_std_output = shared_fd_output_stream(helper::get_stdout());
}

void jkgm::refresh_std_error()
{
    full_std_output = shared_fd_output_stream(helper::get_stderr());
}
