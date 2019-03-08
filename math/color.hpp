#pragma once

#include "abstract_vector.hpp"

namespace jkgm {
    struct color_vector_tag {
    };

    using color = abstract_vector<4, float, color_vector_tag>;
    using color_rgb = abstract_vector<3, float, color_vector_tag>;
    using color_rgba8 = abstract_vector<4, uint8_t, color_vector_tag>;
    using color_rgb8 = abstract_vector<3, uint8_t, color_vector_tag>;

    // Operators
    color_vector_tag operator+(color_vector_tag, color_vector_tag);
    color_vector_tag operator-(color_vector_tag, color_vector_tag);
    color_vector_tag operator*(color_vector_tag, color_vector_tag);
    color_vector_tag operator/(color_vector_tag, color_vector_tag);

    // Swizzles
    struct r {
        static constexpr bool can_return_reference = true;
    };
    struct g {
        static constexpr bool can_return_reference = true;
    };
    struct b {
        static constexpr bool can_return_reference = true;
    };
    struct a {
        static constexpr bool can_return_reference = true;
    };
    struct rgb {
    };

    template <size_t N, class F>
    auto &swizzle(r /*swiz*/, abstract_vector<N, F, color_vector_tag> &v)
    {
        return get<0>(v);
    }

    template <size_t N, class F>
    auto &swizzle(g /*swiz*/, abstract_vector<N, F, color_vector_tag> &v)
    {
        return get<1>(v);
    }

    template <size_t N, class F>
    auto &swizzle(b /*swiz*/, abstract_vector<N, F, color_vector_tag> &v)
    {
        return get<2>(v);
    }

    template <size_t N, class F, class = std::enable_if_t<N >= 4>>
    auto &swizzle(a /*swiz*/, abstract_vector<N, F, color_vector_tag> &v)
    {
        return get<3>(v);
    }

    template <size_t N, class F>
    auto swizzle(r /*swiz*/, abstract_vector<N, F, color_vector_tag> const &v)
    {
        return get<0>(v);
    }

    template <size_t N, class F>
    auto swizzle(g /*swiz*/, abstract_vector<N, F, color_vector_tag> const &v)
    {
        return get<1>(v);
    }

    template <size_t N, class F>
    auto swizzle(b /*swiz*/, abstract_vector<N, F, color_vector_tag> const &v)
    {
        return get<2>(v);
    }

    template <size_t N, class F, class = std::enable_if_t<N >= 4>>
    auto swizzle(a /*swiz*/, abstract_vector<N, F, color_vector_tag> const &v)
    {
        return get<3>(v);
    }

    template <size_t N, class F>
    auto swizzle(rgb /*swiz*/, abstract_vector<N, F, color_vector_tag> const &v)
    {
        return abstract_vector<3, F, color_vector_tag>(get<0>(v), get<1>(v), get<2>(v));
    }

    constexpr auto transparent(color_rgb const &v)
    {
        return extend(v, 0.0f);
    }

    constexpr auto transparent(color_rgb8 const &v)
    {
        return extend(v, static_cast<uint8_t>(0U));
    }

    constexpr auto solid(color_rgb const &v)
    {
        return extend(v, 1.0f);
    }

    constexpr auto solid(color_rgb8 const &v)
    {
        return extend(v, static_cast<uint8_t>(255U));
    }

    namespace detail {
        template <class VecT, size_t N, size_t I>
        struct to_float_color_detail {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const &v, ArgT &&... args)
            {
                return to_float_color_detail<VecT, N, I + 1>::op(
                    v, std::forward<ArgT>(args)..., static_cast<float>(get<I>(v)) / 255.0f);
            }
        };

        template <class VecT, size_t N>
        struct to_float_color_detail<VecT, N, N> {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const & /*v*/, ArgT &&... args)
            {
                return VecT(std::forward<ArgT>(args)...);
            }
        };
    }

    template <size_t N>
    constexpr auto to_float_color(abstract_vector<N, uint8_t, color_vector_tag> const &v)
    {
        return detail::to_float_color_detail<abstract_vector<N, float, color_vector_tag>, N, 0>::op(
            v);
    }

    namespace detail {
        template <class VecT, size_t N, size_t I>
        struct to_discrete_color_detail {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const &v, ArgT &&... args)
            {
                return to_discrete_color_detail<VecT, N, I + 1>::op(
                    v, std::forward<ArgT>(args)..., static_cast<uint8_t>(get<I>(v) * 255.0f));
            }
        };

        template <class VecT, size_t N>
        struct to_discrete_color_detail<VecT, N, N> {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const & /*v*/, ArgT &&... args)
            {
                return VecT(std::forward<ArgT>(args)...);
            }
        };
    }

    template <size_t N>
    constexpr auto to_discrete_color(abstract_vector<N, float, color_vector_tag> const &v)
    {
        return detail::
            to_discrete_color_detail<abstract_vector<N, uint8_t, color_vector_tag>, N, 0>::op(v);
    }

    inline constexpr color_rgb8 operator"" _rgb8(unsigned long long value)
    {
        return color_rgb8(static_cast<uint8_t>(value >> 16U),
                          static_cast<uint8_t>(value >> 8U),
                          static_cast<uint8_t>(value));
    }
}
