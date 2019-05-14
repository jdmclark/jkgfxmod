#pragma once

#include "base/format.hpp"

namespace jkgm {
    void report_error_message(std::string_view msg);
    void report_warning_message(std::string_view msg);
    void report_unimplemented_function(std::string_view name);
}