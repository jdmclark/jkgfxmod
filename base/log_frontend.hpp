#pragma once

#include "diagnostic_context.hpp"
#include "log_midend.hpp"
#include "format.hpp"
#include "local.hpp"
#include <memory>
#include <optional>
#include <stack>

namespace jkgm {
    class log_frontend : public local {
        friend class diagnostic_context;

    private:
        class diagnostic_context_frame {
        public:
            bool referenced = true;
            std::optional<std::shared_ptr<std::string>> filename;
            int first_line;
            int first_col;
            int last_line;
            int last_col;
            int internal_error_count = 0;
            size_t error_count_index;

            diagnostic_context_frame(std::optional<std::shared_ptr<std::string>> filename,
                                     int first_line,
                                     int first_col,
                                     int last_line,
                                     int last_col,
                                     size_t error_count_index);
        };

        std::shared_ptr<log_midend> midend;
        std::vector<diagnostic_context_frame> diagnostic_context;
        bool diagnostic_preamble_dirty = false;
        std::string computed_diagnostic_preamble;

        void update_diagnostic_preamble();

        size_t push_diagnostic_context(std::optional<std::string> filename,
                                       int first_line,
                                       int first_col,
                                       int last_line,
                                       int last_col);

        void release_diagnostic_context(size_t index);

    public:
        explicit log_frontend(local_constructor_protector_tag tag);

        void write_log_message(std::string_view filename,
                               int line_number,
                               log_level level,
                               format_script const &message);

        int diagnostic_file_error_count() const;
        std::string diagnostic_file_name() const;
        size_t get_diagnostic_frame_count() const;
    };

    int diagnostic_file_error_count();
    std::string diagnostic_file_name();
    size_t get_diagnostic_frame_count();
}
