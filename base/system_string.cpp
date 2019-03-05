#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "system_string.hpp"
#include "span.hpp"
#include <codecvt>
#include <locale>

std::string jkgm::detail::ss_to_u8(detail::ss_u16_view sv)
{
    std::wstring_convert<std::codecvt_utf8_utf16<detail::ss_u16_char>, detail::ss_u16_char> wsc;
    return wsc.to_bytes(sv.data(), sv.data() + sv.size()); // NOLINT
}

jkgm::detail::ss_u16_str jkgm::detail::ss_to_u16(std::string_view sv)
{
    std::wstring_convert<std::codecvt_utf8_utf16<detail::ss_u16_char>, detail::ss_u16_char> wsc;
    return wsc.from_bytes(sv.data(), sv.data() + sv.size()); // NOLINT
}

std::string jkgm::escape_windows_cmdline_arg(std::string arg)
{
    if(!arg.empty() && arg.find_first_of(" \t\n\v\"") == std::string::npos) {
        return arg;
    }

    std::string rv;
    rv.reserve(arg.size() + 2);

    rv.push_back('\"');

    size_t num_bs = 0;
    for(char ch : arg) {
        if(ch == '\\') {
            ++num_bs;
        }
        else if(ch == '\"') {
            // Backslashes followed by quote must be escaped.
            rv.append((num_bs * 2) + 1, '\\');
            rv.push_back('\"');
            num_bs = 0;
        }
        else {
            // Backslashes not followed by quote are unspecial
            rv.append(num_bs, '\\');
            rv.push_back(ch);
            num_bs = 0;
        }
    }

    // Backslashes at the end will be followed by a quote, so they should also be escaped.
    rv.append(num_bs * 2, '\\');
    rv.push_back('"');

    return rv;
}

std::string jkgm::argv_to_windows_cmdline(fs::path const &progname, argument_list const &args)
{
    std::string cmdline;
    cmdline.append(escape_windows_cmdline_arg(native_to_utf8(progname.native())));

    for(auto const &arg : *args) {
        cmdline.push_back(' ');
        cmdline.append(escape_windows_cmdline_arg(arg));
    }

    return cmdline;
}
