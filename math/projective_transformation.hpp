#pragma once

#include "affine_transformation.hpp"
#include "box.hpp"

namespace jkgm {
    struct projective_transformation_tag {
    };

    template <size_t M, class F>
    using projective_transformation = abstract_matrix<M, M, F, projective_transformation_tag>;

    // Operators
    projective_transformation_tag operator*(projective_transformation_tag,
                                            affine_transformation_tag);
    projective_transformation_tag operator*(projective_transformation_tag,
                                            projective_transformation_tag);

    // Functions
    namespace detail {
        template <class OutVecT, size_t N, size_t I>
        struct pt_vec_div_n {
            template <class VecT, class... ArgT>
            static inline constexpr auto op(VecT const &v, ArgT &&... arg)
            {
                return pt_vec_div_n<OutVecT, N, I + 1>::op(
                    v, std::forward<ArgT>(arg)..., get<I>(v));
            }
        };

        template <class OutVecT, size_t N>
        struct pt_vec_div_n<OutVecT, N, N> {
            template <class VecT, class... ArgT>
            static inline constexpr auto op(VecT const &v, ArgT &&... arg)
            {
                return OutVecT(std::forward<ArgT>(arg)...) / get<N>(v);
            }
        };
    }

    template <class F, size_t N>
    point<N, F> transform(projective_transformation<N + 1, F> const &m, point<N, F> const &v)
    {
        return detail::pt_vec_div_n<point<N, F>, N, 0>::op(
            detail::mat_vec_mul_detail<point<N + 1, F>, N + 1, N + 1, 0>::op(m, extend(v, F(1))));
    }

    template <class F>
    constexpr auto make_perspective(radians<F> fov_y, F aspect_ratio, F z_near, F z_far) noexcept
    {
        auto tan_half_fov_y = static_cast<F>(tan(fov_y / F(2)));

        F c00 = F(1) / (aspect_ratio * tan_half_fov_y);
        F c11 = F(1) / tan_half_fov_y;
        F c22 = -(z_far + z_near) / (z_far - z_near);
        F c23 = -(F(2) * z_far * z_near) / (z_far - z_near);
        F c32 = -F(1);

        return projective_transformation<4, F>(row(c00, F(0), F(0), F(0)),
                                               row(F(0), c11, F(0), F(0)),
                                               row(F(0), F(0), c22, c23),
                                               row(F(0), F(0), c32, F(0)));
    }

    template <class F>
    constexpr auto make_orthographic(box<3, F> const &extents) noexcept
    {
        auto [left, bottom, z_near] = extents.start;
        auto [right, top, z_far] = extents.stop;

        return projective_transformation<4, F>(
            row(F(2) / (right - left), F(0), F(0), -(right + left) / (right - left)),
            row(F(0), F(2) / (top - bottom), F(0), -(top + bottom) / (top - bottom)),
            row(F(0), F(0), -F(2) / (z_far - z_near), -(z_far + z_near) / (z_far - z_near)),
            row(F(0), F(0), F(0), F(1)));
    }
}
