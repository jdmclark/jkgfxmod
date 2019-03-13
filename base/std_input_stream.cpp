#include "std_input_stream.hpp"
#include "buffered_input_stream.hpp"
#include "fd_input_stream.hpp"
#include "win32.hpp"

namespace jkgm {
    namespace {
        template <platform_family>
        struct helper_impl;

        template <>
        struct helper_impl<platform_family::win32> {
            static win32::handle_id get_stdin()
            {
                return win32::get_std_handle(win32::std_device::std_input);
            }
        };

        using helper = helper_impl<build_traits::family>;

        shared_fd_input_stream full_std_input(helper::get_stdin());
    }
}

jkgm::input_stream &jkgm::std_input = jkgm::full_std_input;

void jkgm::refresh_std_input()
{
    full_std_input = shared_fd_input_stream(helper::get_stdin());
}
