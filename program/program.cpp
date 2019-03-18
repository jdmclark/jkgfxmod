#include "program.hpp"
#include "base/env.hpp"
#include "base/file_log_backend.hpp"
#include "base/std_output_log_backend.hpp"

#include <limits>

jkgm::program::program()
{
    // Initialize logging

    // Logging to standard output shouldn't be too verbose
    jkgm::flag_set<jkgm::log_level> stdout_diagnostic_level{
        jkgm::log_level::error, jkgm::log_level::warning, jkgm::log_level::info};

    if(jkgm::has_environment_variable("JKGM_DEBUG")) {
        stdout_diagnostic_level += jkgm::log_level::debug;
    }

    jkgm::emplace_log_backend<jkgm::std_output_log_backend>(stdout_diagnostic_level);

    // Enable file logging if the correct environment variables are set
    std::optional<std::string> maybe_log_file = jkgm::get_environment_variable("JKGM_LOG_FILE");
    if(maybe_log_file.has_value()) {
        jkgm::emplace_log_backend<jkgm::file_log_backend>({jkgm::log_level::error,
                                                           jkgm::log_level::warning,
                                                           jkgm::log_level::info,
                                                           jkgm::log_level::debug,
                                                           jkgm::log_level::trace},
                                                          maybe_log_file.value());
    }
}

int jkgm::program::start(jkgm::range<char **> const &args)
{
    try {
        create_options(opts);
        opts.load_from_arg_list(args);

        return run();
    }
    catch(std::exception const &e) {
        LOG_ERROR("Exception thrown: ", e.what());
        return EXIT_FAILURE;
    }
    catch(...) {
        LOG_ERROR("Unknown exception thrown");
        return EXIT_FAILURE;
    }
}
