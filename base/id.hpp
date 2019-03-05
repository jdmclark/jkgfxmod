#pragma once

#include <cstdint>
#include <functional>

#define MAKE_ID_TYPE(x, type)                                                                      \
    class x##_id_tag {                                                                             \
    };                                                                                             \
    using x##_id = ::jkgm::id<x##_id_tag, type>

namespace jkgm {
    template <class TagT, class UT>
    class id {
    private:
        UT value;

    public:
        id() = delete;

        explicit constexpr id(UT value) noexcept
            : value(value)
        {
        }

        explicit operator UT() const
        {
            return value;
        }

        constexpr UT get() const
        {
            return value;
        }

        bool operator==(id const &o) const
        {
            return value == o.value;
        }

        bool operator!=(id const &o) const
        {
            return value != o.value;
        }

        bool operator<(id const &o) const
        {
            return value < o.value;
        }

        bool operator<=(id const &o) const
        {
            return value <= o.value;
        }

        bool operator>(id const &o) const
        {
            return value > o.value;
        }

        bool operator>=(id const &o) const
        {
            return value >= o.value;
        }
    };

    template <class RangeT, class TagT, class UT>
    auto const &at(RangeT const &rng, id<TagT, UT> id)
    {
        return rng.at(static_cast<UT>(id));
    }

    template <class RangeT, class TagT, class UT>
    auto &at(RangeT &rng, id<TagT, UT> id)
    {
        return rng.at(static_cast<UT>(id));
    }
}

namespace std {
    template <class TagT, class UT>
    struct underlying_type<::jkgm::id<TagT, UT>> {
        using type = UT;
    };

    template <class TagT, class UT>
    struct hash<::jkgm::id<TagT, UT>> {
    private:
        std::hash<UT> hasher;

    public:
        size_t operator()(::jkgm::id<TagT, UT> const &id) const
        {
            return hasher(static_cast<UT>(id));
        }
    };
}

namespace jkgm {
    namespace detail {
        template <class TagT, class UT>
        void fake_id_function(id<TagT, UT>);
    }

    template <class T>
    struct is_id {
    private:
        using yes = char[1];
        using no = char[2];

        template <class U, class = decltype(detail::fake_id_function(std::declval<U>()))>
        static yes &test(int);

        template <class U>
        static no &test(...);

    public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
}
