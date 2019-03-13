#pragma once

namespace jkgm {
    bool is_space(char ch);
    bool is_digit(char ch);
    bool is_xdigit(char ch);
    bool is_upper(char ch);
    bool is_lower(char ch);
    bool is_alpha(char ch);
    bool is_alnum(char ch);
    bool is_punct(char ch);

    char to_lower(char ch);
}