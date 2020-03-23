#pragma once

#include <functional>
#include <type_traits>

#define MAKE_TAGGED_TYPE(x, type)                                                                  \
    class x##_tag {                                                                                \
    };                                                                                             \
    using x = ::jkgm::tagged<x##_tag, type>

#define MAKE_WEAKLY_TAGGED_TYPE(x, type)                                                           \
    class x##_tag {                                                                                \
    };                                                                                             \
    using x = ::jkgm::weakly_tagged<x##_tag, type>

#define MAKE_TAGGED_RANGE_TYPE(x, type, em)                                                        \
    class x##_tag {                                                                                \
    };                                                                                             \
    using x = ::jkgm::tagged_range<x##_tag, type, em>

#define MAKE_WEAKLY_TAGGED_RANGE_TYPE(x, type, em)                                                 \
    class x##_tag {                                                                                \
    };                                                                                             \
    using x = ::jkgm::weakly_tagged_range<x##_tag, type, em>

namespace jkgm {
    template <class TagT, class UT>
    class tagged {
    protected:
        UT value;

    public:
        using value_type = UT;

        tagged() = default;

        template <class... ArgT>
        explicit tagged(ArgT &&... arg)
            : value(std::forward<ArgT>(arg)...)
        {
        }

        UT &operator*()
        {
            return value;
        }

        UT const &operator*() const
        {
            return value;
        }

        UT *operator->()
        {
            return &value;
        }

        UT const *operator->() const
        {
            return &value;
        }

        bool operator==(tagged const &other) const
        {
            return value == other.value;
        }

        bool operator!=(tagged const &other) const
        {
            return value != other.value;
        }
    };

    template <class TagT, class UT>
    class weakly_tagged : public tagged<TagT, UT> {
    public:
        using tagged<TagT, UT>::tagged;

        weakly_tagged() = default;

        weakly_tagged(UT &&value)
            : tagged<TagT, UT>(std::forward<UT>(value))
        {
        }
    };

    template <class TagT, class UT, class EmT>
    class tagged_range : public tagged<TagT, UT> {
    public:
        tagged_range() = default;

        template <class... ArgT>
        explicit tagged_range(ArgT &&... arg)
            : tagged<TagT, UT>(std::forward<ArgT>(arg)...)
        {
        }

        tagged_range(std::initializer_list<EmT> l)
            : tagged<TagT, UT>(l)
        {
        }
    };

    template <class TagT, class UT, class EmT>
    class weakly_tagged_range : public tagged_range<TagT, UT, EmT> {
    public:
        using tagged_range<TagT, UT, EmT>::tagged_range;

        weakly_tagged_range() = default;

        weakly_tagged_range(UT &&value)
            : tagged_range<TagT, UT, EmT>(std::forward<UT>(value))
        {
        }
    };
}
