#include "format_base.hpp"

jkgm::detail::formatter_fill_op::formatter_fill_op(jkgm::format_buffer *buffer)
    : buffer(buffer)
{
}

jkgm::detail::formatter_sequence_fill_op::formatter_sequence_fill_op(jkgm::format_buffer *buffer)
    : buffer(buffer)
{
}

jkgm::detail::string_format_buffer::string_format_buffer(std::string *buffer)
    : buffer(buffer)
{
}

void jkgm::detail::string_format_buffer::append(std::string_view value)
{
    buffer->append(value);
}
