#pragma once

#include "build_traits.hpp"
#include "input_stream.hpp"
#include "win32.hpp"
#include <optional>

namespace jkgm {
    namespace detail {
        using fd_s_utype = specialization_t<win32_version<win32::handle_id>>;
        using fd_s_type = specialization_t<win32_version<win32::unique_handle>>;
    }

    class shared_fd_input_stream : public input_stream {
    private:
        detail::fd_s_utype fd;

    public:
        explicit shared_fd_input_stream(detail::fd_s_utype fd) noexcept;
        size_t read_some(span<char> dest) override;
    };

    class fd_input_stream : public input_stream {
    private:
        detail::fd_s_type fd;

    public:
        explicit fd_input_stream(detail::fd_s_type &&fd);
        detail::fd_s_utype get() const;
        detail::fd_s_utype release();
        fd_input_stream duplicate() const;
        void close();
        size_t read_some(span<char> dest) override;
    };
}
