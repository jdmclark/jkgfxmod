#pragma once

#include "log_backend.hpp"
#include "format.hpp"
#include "global.hpp"
#include <mutex>
#include <tuple>
#include <vector>

namespace jkgm {
    class log_midend : public global {
    private:
        log_levels present_log_levels;
        std::vector<std::tuple<log_levels, std::unique_ptr<log_backend>>> log_backends;
        std::mutex log_backend_lock;

    public:
        explicit log_midend(global_constructor_protector_tag tag);

        void insert_log_backend(log_levels filter, std::unique_ptr<log_backend> &&b);
        void erase_log_backends();

        void write_log_message(std::string_view filename,
                               int line_number,
                               log_level level,
                               std::string_view diagnostic_context,
                               format_script const &message);
    };
}
