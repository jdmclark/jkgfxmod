#include "std_output_log_backend.hpp"
#include "base/cstring_view.hpp"
#include "format.hpp"
#include "span.hpp"
#include <iostream>

void jkgm::std_output_log_backend::write_message(std::string_view /*filename*/,
                                                 int /*line_number*/,
                                                 log_level level,
                                                 std::string_view message)
{
    std::string_view tag(nullptr, 0);

    switch(level) {
    case log_level::error:
        tag = "[ERROR] ";
        break;

    case log_level::warning:
        tag = "[WARNING] ";
        break;

    case log_level::debug:
        tag = "[DEBUG] ";
        break;

    default:
        break;
    }

    std::cerr << str(format(tag, message, "\n"));
}
