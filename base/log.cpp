#include "log.hpp"

void jkgm::erase_log_backends()
{
    get_global<log_midend>()->erase_log_backends();
}

void jkgm::write_log_message(std::string_view src_filename,
                            int src_line_number,
                            log_level level,
                            format_script const &fmt)
{
    get_local<log_frontend>()->write_log_message(src_filename, src_line_number, level, fmt);
}
