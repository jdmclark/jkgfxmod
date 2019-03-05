#pragma once

#include "abstract_vector.hpp"

namespace jkgm {
    struct direction_vector_tag {
        static constexpr bool has_dimension_swizzle = true;
    };

    template <size_t N, class F>
    using direction = abstract_vector<N, F, direction_vector_tag>;

    template <class C0, class... CN>
    constexpr auto make_direction(C0 &&c0, CN &&... cn) noexcept
    {
        return direction<sizeof...(CN) + 1, std::decay_t<C0>>(std::forward<C0>(c0),
                                                              std::forward<CN>(cn)...);
    }

    // Operators
    direction_vector_tag operator+(direction_vector_tag, direction_vector_tag);
    direction_vector_tag operator-(direction_vector_tag, direction_vector_tag);

    // Swizzles
    struct x {
        static constexpr bool can_return_reference = true;
    };
    struct y {
        static constexpr bool can_return_reference = true;
    };
    struct z {
        static constexpr bool can_return_reference = true;
    };
    struct w {
        static constexpr bool can_return_reference = true;
    };

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 1 && Tag::has_dimension_swizzle>>
    auto &swizzle(x /*swiz*/, abstract_vector<N, F, Tag> &v)
    {
        return get<0>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 2 && Tag::has_dimension_swizzle>>
    auto &swizzle(y /*swiz*/, abstract_vector<N, F, Tag> &v)
    {
        return get<1>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 3 && Tag::has_dimension_swizzle>>
    auto &swizzle(z /*swiz*/, abstract_vector<N, F, Tag> &v)
    {
        return get<2>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 4 && Tag::has_dimension_swizzle>>
    auto &swizzle(w /*swiz*/, abstract_vector<N, F, Tag> &v)
    {
        return get<3>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 1 && Tag::has_dimension_swizzle>>
    auto swizzle(x /*swiz*/, abstract_vector<N, F, Tag> const &v)
    {
        return get<0>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 2 && Tag::has_dimension_swizzle>>
    auto swizzle(y /*swiz*/, abstract_vector<N, F, Tag> const &v)
    {
        return get<1>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 3 && Tag::has_dimension_swizzle>>
    auto swizzle(z /*swiz*/, abstract_vector<N, F, Tag> const &v)
    {
        return get<2>(v);
    }

    template <size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<N >= 4 && Tag::has_dimension_swizzle>>
    auto swizzle(w /*swiz*/, abstract_vector<N, F, Tag> const &v)
    {
        return get<3>(v);
    }
}
