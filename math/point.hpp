#pragma once

#include "direction.hpp"

namespace jkgm {
    struct position_vector_tag {
        static constexpr bool has_dimension_swizzle = true;
    };

    template <size_t N, class F>
    using point = abstract_vector<N, F, position_vector_tag>;

    template <class C0, class... CN>
    constexpr auto make_point(C0 &&c0, CN &&... cn) noexcept
    {
        return point<sizeof...(CN) + 1, std::decay_t<C0>>(std::forward<C0>(c0),
                                                          std::forward<CN>(cn)...);
    }

    // Operators
    position_vector_tag operator+(position_vector_tag, direction_vector_tag);
    position_vector_tag operator-(position_vector_tag, direction_vector_tag);
    direction_vector_tag operator-(position_vector_tag, position_vector_tag);
}
