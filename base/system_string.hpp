#pragma once

#include "build_traits.hpp"
#include "filesystem.hpp"
#include "tagged.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace jkgm {
    namespace detail {
        using ss_u16_char = specialization_t<win32_version<wchar_t>>;
        using ss_u16_str = specialization_t<win32_version<std::wstring>>;
        using ss_u16_view = specialization_t<win32_version<std::wstring_view>>;
    }

    using system_string = specialization_t<win32_version<detail::ss_u16_str>>;
    using system_string_view = specialization_t<win32_version<detail::ss_u16_view>>;

    namespace detail {
        detail::ss_u16_str ss_to_u16(std::string_view sv);
        std::string ss_to_u8(detail::ss_u16_view sv);

        template <platform_family>
        struct ss_converter;

        template <>
        struct ss_converter<platform_family::win32> {
            static std::string native_to_utf8(detail::ss_u16_view sv)
            {
                return ss_to_u8(sv);
            }

            static detail::ss_u16_str utf8_to_native(std::string_view sv)
            {
                return ss_to_u16(sv);
            }
        };

        using ss_conv_spec = ss_converter<build_traits::family>;
    }

    template <class T>
    auto native_to_utf8(T &&sv)
    {
        return detail::ss_conv_spec::native_to_utf8(std::forward<T>(sv));
    }

    template <class T>
    auto utf8_to_native(T &&sv)
    {
        return detail::ss_conv_spec::utf8_to_native(std::forward<T>(sv));
    }

    using argument_list_vec = std::vector<std::string>;
    MAKE_TAGGED_RANGE_TYPE(argument_list, argument_list_vec, std::string);

    std::string escape_windows_cmdline_arg(std::string arg);
    std::string argv_to_windows_cmdline(fs::path const &progname, argument_list const &args);
}
