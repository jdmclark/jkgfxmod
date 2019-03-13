#pragma once

#include "log_backend.hpp"

namespace jkgm {
    class std_output_log_backend : public log_backend {
    public:
        void write_message(std::string_view filename,
                           int line_number,
                           log_level level,
                           std::string_view message) override;
    };
}
