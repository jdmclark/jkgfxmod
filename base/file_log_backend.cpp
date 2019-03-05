#include "file_log_backend.hpp"
#include "pipe.hpp"
#include "format.hpp"

jkgm::file_log_backend::file_log_backend(fs::path const &log_filename)
    : file(std::make_unique<fd_output_stream>(make_output_file_append_pipe(log_filename)))
{
}

void jkgm::file_log_backend::write_message(std::string_view filename,
                                          int line_number,
                                          log_level level,
                                          std::string_view message)
{
    file->write(make_span(
        str(format(filename, "|", line_number, "|", to_string(level), "> ", message, "\n"))));
}
