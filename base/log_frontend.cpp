#include "log_frontend.hpp"
#include "format.hpp"
#include <cassert>
#include <sstream>

jkgm::log_frontend::diagnostic_context_frame::diagnostic_context_frame(
    std::optional<std::shared_ptr<std::string>> filename,
    int first_line,
    int first_col,
    int last_line,
    int last_col,
    size_t error_count_index)
    : filename(std::move(filename))
    , first_line(first_line)
    , first_col(first_col)
    , last_line(last_line)
    , last_col(last_col)
    , error_count_index(error_count_index)
{
}

jkgm::log_frontend::log_frontend(local_constructor_protector_tag tag)
    : local(tag)
    , midend(get_global<log_midend>())
{
}

void jkgm::log_frontend::update_diagnostic_preamble()
{
    diagnostic_preamble_dirty = false;

    if(diagnostic_context.empty()) {
        computed_diagnostic_preamble.clear();
        return;
    }

    auto const &context = diagnostic_context.back();

    std::stringstream ss;
    if(context.filename.has_value()) {
        ss << **context.filename;
    }
    else {
        ss << "<BUFFER>";
    }

    if(context.first_line != 0) {
        ss << ":" << context.first_line;
        if(context.first_col != 0) {
            ss << ":" << context.first_col;
        }

        if(context.last_line != 0) {
            ss << "-" << context.last_line;
            if(context.last_col != 0) {
                ss << ":" << context.last_col;
            }
        }
    }

    std::string partial = ss.str();
    if(!partial.empty()) {
        partial.append(": ");
    }

    computed_diagnostic_preamble = std::move(partial);
}

void jkgm::log_frontend::write_log_message(std::string_view filename,
                                           int line_number,
                                           log_level level,
                                           format_script const &message)
{
    if((level == log_level::error) && !diagnostic_context.empty()) {
        ++diagnostic_context[diagnostic_context.back().error_count_index].internal_error_count;
    }

    if(diagnostic_preamble_dirty) {
        update_diagnostic_preamble();
    }

    midend->write_log_message(filename, line_number, level, computed_diagnostic_preamble, message);
}

size_t jkgm::log_frontend::push_diagnostic_context(std::optional<std::string> filename,
                                                   int first_line,
                                                   int first_col,
                                                   int last_line,
                                                   int last_col)
{
    size_t next_element = diagnostic_context.size();
    size_t error_count_index = next_element;

    std::optional<std::shared_ptr<std::string>> applied_filename;

    if(filename.has_value()) {
        applied_filename = std::make_shared<std::string>(std::move(*filename));
    }
    else if(!filename.has_value() && !diagnostic_context.empty()) {
        applied_filename = diagnostic_context.back().filename;
        error_count_index = diagnostic_context.back().error_count_index;
    }

    diagnostic_context.emplace_back(
        std::move(applied_filename), first_line, first_col, last_line, last_col, error_count_index);
    diagnostic_preamble_dirty = true;

    return next_element;
}

void jkgm::log_frontend::release_diagnostic_context(size_t index)
{
    assert(index < diagnostic_context.size() && "released unallocated diagnostic context");
    assert(diagnostic_context[index].referenced && "released unreferenced diagnostic context");
    diagnostic_context[index].referenced = false;

    while(!diagnostic_context.empty() && !diagnostic_context.back().referenced) {
        diagnostic_context.pop_back();
    }

    diagnostic_preamble_dirty = true;
}

int jkgm::log_frontend::diagnostic_file_error_count() const
{
    if(!diagnostic_context.empty()) {
        return diagnostic_context[diagnostic_context.back().error_count_index].internal_error_count;
    }

    return 0;
}

std::string jkgm::log_frontend::diagnostic_file_name() const
{
    if(diagnostic_context.empty()) {
        return "";
    }

    auto const &context = diagnostic_context.back();
    if(context.filename.has_value()) {
        return **context.filename;
    }

    return "<BUFFER>";
}

size_t jkgm::log_frontend::get_diagnostic_frame_count() const
{
    return diagnostic_context.size();
}

int jkgm::diagnostic_file_error_count()
{
    return get_local<log_frontend>()->diagnostic_file_error_count();
}

std::string jkgm::diagnostic_file_name()
{
    return get_local<log_frontend>()->diagnostic_file_name();
}

size_t jkgm::get_diagnostic_frame_count()
{
    return get_local<log_frontend>()->get_diagnostic_frame_count();
}
