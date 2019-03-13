#include "char.hpp"

namespace {
    template <char lower, char upper>
    bool in_range(char ch)
    {
        static_assert(lower <= upper, "lower end of range must be less than upper");
        static_assert(lower != upper,
                      "lower end of range is equal to upper, consider using equality");
        return lower <= ch && ch <= upper;
    }
}

bool jkgm::is_space(char ch)
{
    return in_range<0x09, 0x0D>(ch) || ch == 0x20;
}

bool jkgm::is_digit(char ch)
{
    return in_range<'0', '9'>(ch);
}

bool jkgm::is_xdigit(char ch)
{
    return in_range<'0', '9'>(ch) || in_range<'a', 'f'>(ch) || in_range<'A', 'F'>(ch);
}

bool jkgm::is_upper(char ch)
{
    return in_range<'A', 'Z'>(ch);
}

bool jkgm::is_lower(char ch)
{
    return in_range<'a', 'z'>(ch);
}

bool jkgm::is_alpha(char ch)
{
    return is_upper(ch) || is_lower(ch);
}

bool jkgm::is_alnum(char ch)
{
    return is_digit(ch) || is_alpha(ch);
}

bool jkgm::is_punct(char ch)
{
    switch(ch) {
    case '!':
    case '\"':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case '-':
    case '.':
    case '/':
    case ':':
    case ';':
    case '<':
    case '=':
    case '>':
    case '?':
    case '@':
    case '[':
    case '\\':
    case ']':
    case '^':
    case '_':
    case '`':
    case '{':
    case '|':
    case '}':
    case '~':
        return true;

    default:
        return false;
    }
}

char jkgm::to_lower(char ch)
{
    if(in_range<'A', 'Z'>(ch)) {
        return ('a' - 'A') + ch;
    }

    return ch;
}
