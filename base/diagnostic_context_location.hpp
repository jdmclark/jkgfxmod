#pragma once

#include <optional>
#include <string>

namespace jkgm {
    class diagnostic_context_location {
    public:
        std::optional<std::string> filename;
        int first_line = 0;
        int first_col = 0;
        int last_line = 0;
        int last_col = 0;

        diagnostic_context_location() = default;
        explicit diagnostic_context_location(std::optional<std::string> filename,
                                             int first_line,
                                             int first_col,
                                             int last_line = 0,
                                             int last_col = 0);

        bool operator==(diagnostic_context_location const &loc) const;
    };

    diagnostic_context_location location_union(diagnostic_context_location start,
                                               diagnostic_context_location const &stop);
}
