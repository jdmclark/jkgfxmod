#pragma once

namespace jkgm {
    template <class F>
    struct pi;

    template <>
    struct pi<float> {
        static constexpr float value = 3.14159265358979323846264338327950288f;
    };

    template <>
    struct pi<double> {
        static constexpr double value = 3.14159265358979323846264338327950288;
    };

    template <class F>
    inline constexpr F pi_v = pi<F>::value;
}
