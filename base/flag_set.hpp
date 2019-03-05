#pragma once

#include <initializer_list>
#include <type_traits>

namespace jkgm {
    template <class EnumT>
    class flag_set {
        using UT = typename std::underlying_type<EnumT>::type;
        static_assert(std::is_unsigned_v<UT>, "flag_set underlying type must be unsigned");

    private:
        UT value;

    public:
        constexpr flag_set() noexcept
            : value(0)
        {
        }

        constexpr explicit flag_set(UT forced_value) noexcept
            : value(forced_value)
        {
        }

        constexpr flag_set(std::initializer_list<EnumT> initl) noexcept
            : value(0)
        {
            for(EnumT val : initl) {
                value |= static_cast<UT>(val);
            }
        }

        explicit operator EnumT() const
        {
            return EnumT(value);
        }

        explicit operator UT() const
        {
            return value;
        }

        inline bool operator&(EnumT val) const
        {
            return value & static_cast<UT>(val);
        }

        inline bool operator&(flag_set val) const
        {
            return value & val.value;
        }

        inline flag_set operator+(EnumT val) const
        {
            return flag_set(value | static_cast<UT>(val));
        }

        inline flag_set operator+(flag_set val) const
        {
            return flag_set(value | val.value);
        }

        inline flag_set const &operator+=(EnumT val)
        {
            value |= static_cast<UT>(val);
            return *this;
        }

        inline flag_set const &operator+=(flag_set val)
        {
            value |= val.value;
            return *this;
        }

        inline flag_set operator-(EnumT val) const
        {
            return flag_set(value & ~static_cast<UT>(val));
        }

        inline flag_set operator-(flag_set val) const
        {
            return flag_set(value & ~val.value);
        }

        inline flag_set const &operator-=(EnumT val)
        {
            value = value & ~static_cast<UT>(val);
            return *this;
        }

        inline flag_set const &operator-=(flag_set val)
        {
            value = value & ~val.value;
            return *this;
        }

        inline bool operator==(flag_set val) const
        {
            return value == val.value;
        }

        inline bool operator!=(flag_set val) const
        {
            return value != val.value;
        }
    };

    namespace detail {
        template <class U>
        U fake_flag_set_enum_type(flag_set<U> const &);
    }

    template <class T>
    struct flag_set_enum_type {
        using type = decltype(detail::fake_flag_set_enum_type(std::declval<T>()));
    };

    template <class T>
    struct flag_set_underlying_type {
        using type = typename std::underlying_type<typename flag_set_enum_type<T>::type>::type;
    };

    template <class T>
    struct is_flag_set {
    private:
        using yes = char[1];
        using no = char[2];

        template <class U, class = decltype(detail::fake_flag_set_enum_type(std::declval<U>()))>
        static yes &test(int);

        template <class U>
        static no &test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
}
