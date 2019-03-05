#pragma once

#include "abstract_vector.hpp"
#include "constants.hpp"
#include <cmath>

namespace jkgm {
    struct degree_angle_tag {
    };

    template <class F>
    using degrees = abstract_scalar<F, degree_angle_tag>;

    struct radian_angle_tag {
    };

    template <class F>
    using radians = abstract_scalar<F, radian_angle_tag>;

    template <class F>
    constexpr auto to_radians(degrees<F> angle) noexcept
    {
        return radians<F>(get(angle) * F(pi_v<double> / 180.0));
    }

    template <class F>
    constexpr auto to_degrees(radians<F> angle) noexcept
    {
        return degrees<F>(get(angle) * F(180.0 / pi_v<double>));
    }

    // Operators
    degree_angle_tag operator+(degree_angle_tag, degree_angle_tag);
    degree_angle_tag operator-(degree_angle_tag, degree_angle_tag);
    radian_angle_tag operator+(radian_angle_tag, radian_angle_tag);
    radian_angle_tag operator-(radian_angle_tag, radian_angle_tag);

    // Helpers
    template <class F>
    constexpr auto sin(radians<F> angle) noexcept
    {
        return std::sin(get(angle));
    }

    template <class F>
    constexpr auto sin(degrees<F> angle) noexcept
    {
        return sin(to_radians(angle));
    }

    template <class F>
    constexpr auto asin(F ratio) noexcept
    {
        return radians<F>(std::asin(ratio));
    }

    template <class F>
    constexpr auto cos(radians<F> angle) noexcept
    {
        return std::cos(get(angle));
    }

    template <class F>
    constexpr auto cos(degrees<F> angle) noexcept
    {
        return cos(to_radians(angle));
    }

    template <class F>
    constexpr auto acos(F ratio) noexcept
    {
        return radians<F>(std::acos(ratio));
    }

    template <class F>
    constexpr auto tan(radians<F> angle) noexcept
    {
        return std::tan(get(angle));
    }

    template <class F>
    constexpr auto tan(degrees<F> angle) noexcept
    {
        return tan(to_radians(angle));
    }
}
