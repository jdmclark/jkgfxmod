#pragma once

#include "diagnostic_context_location.hpp"
#include <cstddef>
#include <optional>

namespace jkgm {
    class diagnostic_context {
    private:
        std::optional<size_t> diagnostic_context_handle;

    public:
        explicit diagnostic_context(diagnostic_context_location loc);
        explicit diagnostic_context(std::optional<std::string> filename,
                                    int first_line = 0,
                                    int first_col = 0,
                                    int last_line = 0,
                                    int last_col = 0);

        void release();
        ~diagnostic_context();

        diagnostic_context(diagnostic_context const &) = delete;
        diagnostic_context(diagnostic_context &&) = delete;
        diagnostic_context &operator=(diagnostic_context const &) = delete;
        diagnostic_context &operator=(diagnostic_context &&) = delete;
    };
}
