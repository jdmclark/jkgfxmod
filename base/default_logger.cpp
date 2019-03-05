#include "default_logger.hpp"
#include "env.hpp"
#include "file_log_backend.hpp"
#include "log.hpp"

void jkgm::setup_default_logging()
{
    // Initialize logging
    auto maybe_log_file = get_environment_variable("JKGM_LOG_FILE");
    if(maybe_log_file.has_value()) {
        emplace_log_backend<file_log_backend>({log_level::error,
                                               log_level::warning,
                                               log_level::info,
                                               log_level::debug,
                                               log_level::trace},
                                              *maybe_log_file);
    }
}