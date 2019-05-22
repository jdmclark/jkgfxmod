#pragma once

#include "base/format.hpp"

namespace jkgm {
    [[noreturn]] void report_fatal_message(std::string_view msg);
    void report_error_message(std::string_view msg);
    void report_warning_message(std::string_view msg);

    [[noreturn]] void report_unimplemented_function(std::string_view name);
}