#pragma once

#include "abstract_matrix.hpp"
#include "angle.hpp"
#include "constants.hpp"
#include "direction.hpp"
#include "point.hpp"

namespace jkgm {
    struct linear_transformation_tag {
    };

    template <size_t M, size_t N, class F>
    using linear_transformation = abstract_matrix<M, N, F, linear_transformation_tag>;

    // Operators
    linear_transformation_tag operator*(linear_transformation_tag, linear_transformation_tag);
    position_vector_tag operator*(linear_transformation_tag, position_vector_tag);
    direction_vector_tag operator*(linear_transformation_tag, direction_vector_tag);

    // Functions
    template <size_t N, class F>
    constexpr auto
        make_scale(typename linear_transformation<N, N, F>::diag_vector const &d) noexcept
    {
        return linear_transformation<N, N, F>::diagonal(d);
    }

    template <size_t N, class F>
    constexpr auto make_uniform_scale(F c) noexcept
    {
        return linear_transformation<N, N, F>::diagonal(
            linear_transformation<N, N, F>::diag_vector::fill(c));
    }

    template <class F>
    constexpr linear_transformation<3, 3, F> make_rotation_matrix(radians<F> angle,
                                                                  direction<3, F> axis) noexcept
    {
        axis = normalize(axis);

        auto c = static_cast<F>(cos(angle));
        auto s = static_cast<F>(sin(angle));
        auto temp = (F(1) - c) * axis;

        F c00 = c + get<x>(temp) * get<x>(axis);
        F c01 = 0 + get<y>(temp) * get<x>(axis) - s * get<z>(axis);
        F c02 = 0 + get<z>(temp) * get<x>(axis) + s * get<y>(axis);

        F c10 = 0 + get<x>(temp) * get<y>(axis) + s * get<z>(axis);
        F c11 = c + get<y>(temp) * get<y>(axis);
        F c12 = 0 + get<z>(temp) * get<y>(axis) - s * get<x>(axis);

        F c20 = 0 + get<x>(temp) * get<z>(axis) - s * get<y>(axis);
        F c21 = 0 + get<y>(temp) * get<z>(axis) + s * get<x>(axis);
        F c22 = c + get<z>(temp) * get<z>(axis);

        return linear_transformation<3, 3, F>(
            row(c00, c01, c02), row(c10, c11, c12), row(c20, c21, c22));
    }

    template <class F>
    constexpr auto invert(linear_transformation<2, 2, F> const &m) noexcept
    {
        auto one_over_det = F(1) / determinant(m);
        auto [a, b] = get_row<0>(m);
        auto [c, d] = get_row<1>(m);

        return one_over_det * linear_transformation<2, 2, F>(row(d, -b), row(-c, a));
    }

    template <class F>
    constexpr auto invert(linear_transformation<3, 3, F> const &m) noexcept
    {
        auto one_over_det = F(1) / determinant(m);

        auto [a, b, c] = get_row<0>(m);
        auto [d, e, f] = get_row<1>(m);
        auto [g, h, i] = get_row<2>(m);

        return linear_transformation<3, 3, F>(
                   row(+(e * i - f * h), -(b * i - c * h), +(b * f - c * e)),
                   row(-(d * i - f * g), +(a * i - c * g), -(a * f - c * d)),
                   row(+(d * h - e * g), -(a * h - b * g), +(a * e - b * d))) *
               one_over_det;
    }
}
