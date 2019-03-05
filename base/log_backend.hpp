#pragma once

#include "log_level.hpp"
#include <string_view>

namespace jkgm {
    class log_backend {
    protected:
        log_backend() = default;

    public:
        virtual ~log_backend() = default;

        log_backend(log_backend const &) = delete;
        log_backend(log_backend &&) = delete;
        log_backend &operator=(log_backend const &) = delete;
        log_backend &operator=(log_backend &&) = delete;

        virtual void write_message(std::string_view filename,
                                   int line_number,
                                   log_level level,
                                   std::string_view message) = 0;
    };
}
