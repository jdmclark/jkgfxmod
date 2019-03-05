#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

namespace jkgm {
    template <class F, class = void>
    struct almost_equal_traits;

    template <class F>
    struct almost_equal_traits<F, std::enable_if_t<std::is_floating_point_v<F>>> {
        using field_type = F;
        static constexpr auto default_threshold = F(1) * std::numeric_limits<F>::epsilon();
        static constexpr auto default_ulps = F(2) * std::numeric_limits<F>::epsilon();

        static inline constexpr bool almost_equal(F x, F y, F maxdiff, F reldiff)
        {
            auto absxmy = std::abs(x - y);
            return (absxmy <= maxdiff) || (absxmy <= (std::abs(x) + std::abs(y)) * reldiff);
        }
    };

    template <class F>
    struct almost_equal_traits<F, std::enable_if_t<std::is_integral_v<F>>> {
        using field_type = F;
        static constexpr auto default_threshold = F(0);
        static constexpr auto default_ulps = F(0);

        static inline constexpr bool almost_equal(F x, F y, F /*maxdiff*/, F /*reldiff*/)
        {
            return x == y;
        }
    };

    template <class T, class TraitsT = almost_equal_traits<std::decay_t<T>>>
    bool almost_equal(T const &x,
                      T const &y,
                      typename TraitsT::field_type maxdiff = TraitsT::default_threshold,
                      typename TraitsT::field_type reldiff = TraitsT::default_ulps)
    {
        return TraitsT::almost_equal(x, y, maxdiff, reldiff);
    }
}
