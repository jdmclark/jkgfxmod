#include "diagnostic_context_location.hpp"
#include <cassert>
#include <tuple>

jkgm::diagnostic_context_location::diagnostic_context_location(std::optional<std::string> filename,
                                                               int first_line,
                                                               int first_col,
                                                               int last_line,
                                                               int last_col)
    : filename(std::move(filename))
    , first_line(first_line)
    , first_col(first_col)
    , last_line(last_line)
    , last_col(last_col)
{
}

bool jkgm::diagnostic_context_location::operator==(diagnostic_context_location const &loc) const
{
    return std::tie(filename, first_line, first_col, last_line, last_col) ==
           std::tie(loc.filename, loc.first_line, loc.first_col, loc.last_line, loc.last_col);
}

jkgm::diagnostic_context_location jkgm::location_union(diagnostic_context_location start,
                                                       diagnostic_context_location const &stop)
{
    assert((start.filename == stop.filename || !stop.filename.has_value()) &&
           "cannot union diagnostic context locations from two different file contexts");
    return diagnostic_context_location(std::move(start.filename),
                                       start.first_line,
                                       start.first_col,
                                       stop.last_line,
                                       stop.last_col);
}
