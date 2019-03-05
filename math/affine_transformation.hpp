#pragma once

#include "linear_transformation.hpp"

namespace jkgm {
    struct affine_transformation_tag {
    };

    template <size_t M, class F>
    using affine_transformation = abstract_matrix<M, M, F, affine_transformation_tag>;

    // Operators
    affine_transformation_tag operator*(affine_transformation_tag, affine_transformation_tag);

    // Functions

    namespace detail {
        template <class OutVecT, size_t N, size_t I>
        struct at_vec_drop_n {
            template <class VecT, class... ArgT>
            static inline constexpr auto op(VecT const &v, ArgT &&... arg)
            {
                return at_vec_drop_n<OutVecT, N, I + 1>::op(
                    v, std::forward<ArgT>(arg)..., get<I>(v));
            }
        };

        template <class OutVecT, size_t N>
        struct at_vec_drop_n<OutVecT, N, N> {
            template <class VecT, class... ArgT>
            static inline constexpr auto op(VecT const & /*v*/, ArgT &&... arg)
            {
                return OutVecT(std::forward<ArgT>(arg)...);
            }
        };
    }

    template <class F, size_t N>
    point<N, F> transform(affine_transformation<N + 1, F> const &m, point<N, F> const &v)
    {
        return detail::at_vec_drop_n<point<N, F>, N, 0>::op(
            detail::mat_vec_mul_detail<point<N + 1, F>, N + 1, N + 1, 0>::op(m, extend(v, F(1))));
    }

    template <class F, size_t N>
    direction<N, F> transform(affine_transformation<N + 1, F> const &m, direction<N, F> const &v)
    {
        return detail::at_vec_drop_n<direction<N, F>, N, 0>::op(
            detail::mat_vec_mul_detail<direction<N + 1, F>, N + 1, N + 1, 0>::op(m,
                                                                                 extend(v, F(0))));
    }

    namespace detail {
        template <class MatT, size_t N, size_t I>
        struct make_affine_detail {
            template <class LinT, class TransT, class... RowT>
            static constexpr auto
                op(LinT const &linear_part, TransT const &translation, RowT &&... rows) noexcept
            {
                return make_affine_detail<MatT, N, I + 1>::op(linear_part,
                                                              translation,
                                                              std::forward<RowT>(rows)...,
                                                              extend(get_row<I>(linear_part),
                                                                     get<I>(translation)));
            }
        };

        template <class MatT, size_t N>
        struct make_affine_detail<MatT, N, N> {
            template <class LinT, class TransT, class... RowT>
            static constexpr auto op(LinT const & /*linear_part*/,
                                     TransT const & /*translation*/,
                                     RowT &&... rows) noexcept
            {
                return MatT(std::forward<RowT>(rows)...,
                            extend(LinT::row_vector::zero(), typename TransT::field_type(1)));
            }
        };
    }

    template <size_t N, class F>
    constexpr auto make_affine(linear_transformation<N, N, F> const &linear_part,
                               direction<N, F> translation = direction<N, F>::zero()) noexcept
    {
        return detail::make_affine_detail<affine_transformation<N + 1, F>, N, 0>::op(linear_part,
                                                                                     translation);
    }

    template <size_t N, class F>
    constexpr auto make_translation(direction<N, F> translation) noexcept
    {
        return make_affine(linear_transformation<N, N, F>::identity(), translation);
    }

    template <class F>
    constexpr auto make_look(point<3, F> eye, direction<3, F> forward, direction<3, F> up)
    {
        auto f = normalize(forward);
        auto s = normalize(cross(f, up));
        auto u = cross(s, f);

        return affine_transformation<4, F>(row(get<x>(s), get<y>(s), get<z>(s), -dot(s, eye)),
                                           row(get<x>(u), get<y>(u), get<z>(u), -dot(u, eye)),
                                           row(-get<x>(f), -get<y>(f), -get<z>(f), dot(f, eye)),
                                           row(F(0), F(0), F(0), F(1)));
    }

    template <class F>
    constexpr auto make_model_look(point<3, F> pos, direction<3, F> forward, direction<3, F> up)
    {
        auto f = normalize(forward);
        auto s = normalize(cross(f, up));
        auto u = cross(s, f);

        return make_translation(pos - point<3, F>::zero()) *
               make_affine(linear_transformation<3, 3, F>(row(get<x>(s), get<x>(u), get<x>(f)),
                                                          row(get<y>(s), get<y>(u), get<y>(f)),
                                                          row(get<z>(s), get<z>(u), get<z>(f))));
    }

    template <class F>
    constexpr auto invert(affine_transformation<4, F> const &m)
    {
        auto [m00, m01, m02, m03] = get_row<0>(m);
        auto [m10, m11, m12, m13] = get_row<1>(m);
        auto [m20, m21, m22, m23] = get_row<2>(m);
        auto [m30, m31, m32, m33] = get_row<3>(m);

        auto lin_part = invert(linear_transformation<3, 3, F>(
            row(m00, m01, m02), row(m10, m11, m12), row(m20, m21, m22)));

        auto [dir0, dir1, dir2] = lin_part * -make_direction(m03, m13, m23);

        return affine_transformation<4, F>(extend(get_row<0>(lin_part), dir0),
                                           extend(get_row<1>(lin_part), dir1),
                                           extend(get_row<2>(lin_part), dir2),
                                           row(m30, m31, m32, m33));
    }
}
