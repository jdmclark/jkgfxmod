#include "log_midend.hpp"

jkgm::log_midend::log_midend(global_constructor_protector_tag tag)
    : global(tag)
{
}

void jkgm::log_midend::insert_log_backend(log_levels filter, std::unique_ptr<log_backend> &&b)
{
    std::lock_guard<std::mutex> lock(log_backend_lock);
    log_backends.emplace_back(filter, std::move(b));
    present_log_levels += filter;
}

void jkgm::log_midend::erase_log_backends()
{
    std::lock_guard<std::mutex> lock(log_backend_lock);
    present_log_levels = {};
    log_backends.clear();
}

void jkgm::log_midend::write_log_message(std::string_view filename,
                                        int line_number,
                                        log_level level,
                                        std::string_view diagnostic_context,
                                        format_script const &message)
{
    std::lock_guard<std::mutex> lock(log_backend_lock);

    // Check backends to see if the message needs to be formatted
    if(!(present_log_levels & level)) {
        return;
    }

    auto msg = str(format(diagnostic_context, message));

    for(auto &b : log_backends) {
        if(std::get<0>(b) & level) {
            std::get<1>(b)->write_message(filename, line_number, level, msg);
        }
    }
}
