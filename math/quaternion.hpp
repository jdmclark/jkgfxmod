#pragma once

#include "lerp.hpp"
#include "linear_transformation.hpp"

namespace jkgm {
    struct quaternion_vector_tag {
    };

    template <class F>
    using quaternion = abstract_vector<4, F, quaternion_vector_tag>;

    // Operators
    position_vector_tag transform(quaternion_vector_tag, position_vector_tag) noexcept;
    direction_vector_tag transform(quaternion_vector_tag, direction_vector_tag) noexcept;

    template <class F>
    constexpr auto operator*(quaternion<F> const &q0, quaternion<F> const &q1) noexcept
    {
        return quaternion<F>(get<0>(q0) * get<0>(q1) - get<1>(q0) * get<1>(q1) -
                                 get<2>(q0) * get<2>(q1) - get<3>(q0) * get<3>(q1),
                             get<0>(q0) * get<1>(q1) + get<1>(q0) * get<0>(q1) +
                                 get<2>(q0) * get<3>(q1) - get<3>(q0) * get<2>(q1),
                             get<0>(q0) * get<2>(q1) - get<1>(q0) * get<3>(q1) +
                                 get<2>(q0) * get<0>(q1) + get<3>(q0) * get<1>(q1),
                             get<0>(q0) * get<3>(q1) + get<1>(q0) * get<2>(q1) -
                                 get<2>(q0) * get<1>(q1) + get<3>(q0) * get<0>(q1));
    }

    template <class F>
    constexpr auto conjugate(quaternion<F> const &q) noexcept
    {
        return quaternion<F>(get<0>(q), -get<1>(q), -get<2>(q), -get<3>(q));
    }

    template <class F, class Tag>
    constexpr auto transform(quaternion<F> const &q, abstract_vector<3, F, Tag> const &v) noexcept
    {
        quaternion<F> vq(F(0), get<0>(v), get<1>(v), get<2>(v));
        auto rq = (q * vq * conjugate(q)) / length_squared(q);
        return abstract_vector<3, F, decltype(transform(quaternion_vector_tag(), Tag()))>(
            get<1>(rq), get<2>(rq), get<3>(rq));
    }

    template <class F>
    constexpr auto make_rotation_quaternion(radians<F> angle, direction<3, F> axis) noexcept
    {
        auto s = static_cast<F>(sin(angle / F(2)));
        auto c = static_cast<F>(cos(angle / F(2)));
        return quaternion<F>(c, get<0>(axis) * s, get<1>(axis) * s, get<2>(axis) * s);
    }

    template <class F>
    constexpr auto identity_quaternion = quaternion<F>(F(1), F(0), F(0), F(0));

    template <class F>
    constexpr auto make_rotation_matrix(quaternion<F> const &q) noexcept
    {
        auto [w, x, y, z] = q;
        auto qxx = x * x;
        auto qyy = y * y;
        auto qzz = z * z;
        auto qxz = x * z;
        auto qxy = x * y;
        auto qyz = y * z;
        auto qwx = w * x;
        auto qwy = w * y;
        auto qwz = w * z;

        return linear_transformation<3, 3, F>(
            row(F(1) - F(2) * (qyy + qzz), F(2) * (qxy - qwz), F(2) * (qxz + qwy)),
            row(F(2) * (qxy + qwz), F(1) - F(2) * (qxx + qzz), F(2) * (qyz - qwx)),
            row(F(2) * (qxz - qwy), F(2) * (qyz + qwx), F(1) - F(2) * (qxx + qyy)));
    }

    template <class F>
    constexpr auto slerp(quaternion<F> a, quaternion<F> b, F alpha) noexcept
    {
        double cos_half_theta = dot(a, b);

        // Enforce short rotation
        if(cos_half_theta < 0.0) {
            cos_half_theta = -cos_half_theta;
            a = -a;
        }

        double half_theta = std::acos(cos_half_theta);
        double sin_half_theta = std::sqrt(1.0 - cos_half_theta * cos_half_theta);

        if(std::abs(sin_half_theta) < 0.001) {
            return lerp(a, b, F(0.5));
        }

        auto ratio_a = static_cast<F>(std::sin((1.0 - static_cast<double>(alpha)) * half_theta) /
                                      sin_half_theta);
        auto ratio_b =
            static_cast<F>(std::sin(static_cast<double>(alpha) * half_theta) / sin_half_theta);

        return force_retag<quaternion_vector_tag>(force_retag<direction_vector_tag>(a * ratio_a) +
                                                  force_retag<direction_vector_tag>(b * ratio_b));
    }

    template <class F>
    constexpr auto invert(quaternion<F> const &q) noexcept
    {
        return conjugate(q) / length_squared(q);
    }
}
