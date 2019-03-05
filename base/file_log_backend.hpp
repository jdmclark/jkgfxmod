#pragma once

#include "filesystem.hpp"
#include "log_backend.hpp"
#include "output_stream.hpp"
#include <memory>

namespace jkgm {
    class file_log_backend : public log_backend {
    private:
        std::unique_ptr<output_stream> file;

    public:
        explicit file_log_backend(fs::path const &log_filename);

        void write_message(std::string_view filename,
                           int line_number,
                           log_level level,
                           std::string_view message) override;
    };
}
