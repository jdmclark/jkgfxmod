#include "diagnostic_context.hpp"
#include "log_frontend.hpp"

jkgm::diagnostic_context::diagnostic_context(diagnostic_context_location loc)
{
    diagnostic_context_handle = get_local<log_frontend>()->push_diagnostic_context(
        std::move(loc.filename), loc.first_line, loc.first_col, loc.last_line, loc.last_col);
}

jkgm::diagnostic_context::diagnostic_context(std::optional<std::string> filename,
                                            int first_line,
                                            int first_col,
                                            int last_line,
                                            int last_col)
{
    diagnostic_context_handle = get_local<log_frontend>()->push_diagnostic_context(
        std::move(filename), first_line, first_col, last_line, last_col);
}

void jkgm::diagnostic_context::release()
{
    if(diagnostic_context_handle.has_value()) {
        get_local<log_frontend>()->release_diagnostic_context(*diagnostic_context_handle);
        diagnostic_context_handle.reset();
    }
}

jkgm::diagnostic_context::~diagnostic_context()
{
    try {
        release();
    }
    catch(...) {
        // Nothing to do
    }
}
