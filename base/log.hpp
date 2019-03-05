#pragma once

#include "format.hpp"
#include "log_backend.hpp"
#include "log_frontend.hpp"
#include "log_midend.hpp"
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>

namespace jkgm {
    template <class T, class... ArgT>
    void emplace_log_backend(log_levels filter, ArgT &&... arg)
    {
        get_global<log_midend>()->insert_log_backend(
            filter, std::make_unique<T>(std::forward<ArgT>(arg)...));
    }

    void erase_log_backends();

    void write_log_message(std::string_view src_filename,
                           int src_line_number,
                           log_level level,
                           format_script const &fmt);
}

#define LOG_WITH_LEVEL(level, ...)                                                                 \
    ::jkgm::write_log_message(__FILE__, __LINE__, (level), (::jkgm::format(__VA_ARGS__)))

#define LOG_ERROR(...) LOG_WITH_LEVEL(::jkgm::log_level::error, __VA_ARGS__)

#define LOG_WARNING(...) LOG_WITH_LEVEL(::jkgm::log_level::warning, __VA_ARGS__)

#define LOG_INFO(...) LOG_WITH_LEVEL(::jkgm::log_level::info, __VA_ARGS__)

#define LOG_DEBUG(...) LOG_WITH_LEVEL(::jkgm::log_level::debug, __VA_ARGS__)

#define LOG_TRACE(...) LOG_WITH_LEVEL(::jkgm::log_level::trace, __VA_ARGS__)
