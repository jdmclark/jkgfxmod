#pragma once

namespace jkgm {
    template <class F, class T>
    constexpr T lerp(T x0, T x1, F alpha)
    {
        return x0 + ((x1 - x0) * alpha);
    }

    template <class T>
    constexpr auto invlerp(T x0, T x1, T value)
    {
        return (value - x0) / (x1 - x0);
    }
}
