#pragma once

#include "direction.hpp"

namespace jkgm {
    struct size_vector_tag {
        static constexpr bool has_dimension_swizzle = true;
    };

    template <size_t N, class F>
    using size = abstract_vector<N, F, size_vector_tag>;

    template <class C0, class... CN>
    constexpr auto make_size(C0 &&c0, CN &&... cn) noexcept
    {
        return size<sizeof...(CN) + 1, std::decay_t<C0>>(std::forward<C0>(c0),
                                                         std::forward<CN>(cn)...);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct volume_detail {
            template <class VecT, class F>
            static inline constexpr auto op(VecT const &v, F c)
            {
                return volume_detail<N, I + 1>::op(v, c * get<I>(v));
            }
        };

        template <size_t N>
        struct volume_detail<N, N> {
            template <class VecT, class F>
            static inline constexpr auto op(VecT const & /*v*/, F c)
            {
                return c;
            }
        };
    }

    template <size_t N, class F>
    constexpr F volume(size<N, F> const &v)
    {
        return detail::volume_detail<N, 0>::op(v, F(1));
    }

    namespace detail {
        template <size_t N, size_t I>
        struct is_positive_detail {
            template <class VecT, class F>
            static inline constexpr auto op(VecT const &v)
            {
                if(get<I>(v) < F(0)) {
                    return false;
                }

                return is_positive_detail<N, I + 1>::template op<VecT, F>(v);
            }
        };

        template <size_t N>
        struct is_positive_detail<N, N> {
            template <class VecT, class F>
            static inline constexpr auto op(VecT const & /*v*/)
            {
                return true;
            }
        };
    }

    template <size_t N, class F>
    constexpr bool is_positive(size<N, F> const &v)
    {
        return detail::is_positive_detail<N, 0>::template op<size<N, F>, F>(v);
    }
}
