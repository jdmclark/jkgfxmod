#pragma once

#include "build_traits.hpp"
#include "output_stream.hpp"
#include "win32.hpp"
#include <optional>

namespace jkgm {
    namespace detail {
        using fd_os_utype = specialization_t<win32_version<win32::handle_id>>;
        using fd_os_type = specialization_t<win32_version<win32::unique_handle>>;
    }

    class shared_fd_output_stream : public output_stream {
    private:
        detail::fd_os_utype fd;

    public:
        explicit shared_fd_output_stream(detail::fd_os_utype fd);
        size_t write_some(span<char const> src) override;
    };

    class fd_output_stream : public output_stream {
    private:
        detail::fd_os_type fd;

    public:
        explicit fd_output_stream(detail::fd_os_type &&fd);
        detail::fd_os_utype get() const;
        detail::fd_os_utype release();
        fd_output_stream duplicate() const;
        void close();
        size_t write_some(span<char const> dest) override;
    };
}
