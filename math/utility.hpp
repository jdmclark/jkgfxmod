#pragma once

#include "abstract_vector.hpp"
#include "box.hpp"

namespace jkgm {
    // Clamp
    namespace detail {
        template <class F, class B>
        inline constexpr B inner_clamp(F &&value, B min, B max) noexcept
        {
            if(value < min) {
                return std::forward<F>(min);
            }

            if(max < value) {
                return std::forward<F>(max);
            }

            return std::forward<F>(value);
        }

        template <class T, size_t D, size_t I>
        struct clamp_detail {
            template <class VarT, class BoxT>
            static inline constexpr auto op(VarT &&value, BoxT const &b) noexcept
            {
                return inner_clamp(std::forward<VarT>(value), get<0>(b.start), get<0>(b.stop));
            }
        };

        template <class F, class TagT, size_t D, size_t I>
        struct clamp_detail<abstract_vector<D, F, TagT>, D, I> {
            template <class BoxT, class... ArgT>
            static inline constexpr auto op(abstract_vector<D, F, TagT> const &value,
                                            BoxT const &b,
                                            ArgT &&... args) noexcept
            {
                return clamp_detail<abstract_vector<D, F, TagT>, D, I + 1>::op(
                    value,
                    b,
                    std::forward<ArgT>(args)...,
                    inner_clamp(get<I>(value), get<I>(b.start), get<I>(b.stop)));
            }
        };

        template <class F, class TagT, size_t D>
        struct clamp_detail<abstract_vector<D, F, TagT>, D, D> {
            template <class BoxT, class... ArgT>
            static inline constexpr auto op(abstract_vector<D, F, TagT> const & /*value*/,
                                            BoxT const & /*b*/,
                                            ArgT &&... args) noexcept
            {
                return abstract_vector<D, F, TagT>(std::forward<ArgT>(args)...);
            }
        };
    }

    template <class T, size_t D, class F>
    constexpr auto clamp(T &&value, box<D, F> const &b) noexcept
    {
        return detail::clamp_detail<std::decay_t<T>, D, 0>::op(std::forward<T>(value), b);
    }

    // Linear interpolation
    namespace detail {
        template <class T>
        inline constexpr T inner_lerp(T x0, T x1, T alpha) noexcept
        {
            return x0 + ((x1 - x0) * alpha);
        }

        template <size_t D, size_t I>
        struct vec_lerp_detail {
            template <class EdgeT, class AlphaT, class... Args>
            static inline constexpr auto
                op(EdgeT const &x0, EdgeT const &x1, AlphaT alpha, Args &&... args)
            {
                if constexpr(I < D) { // NOLINT fp
                    return vec_lerp_detail<D, I + 1>::op(x0,
                                                         x1,
                                                         alpha,
                                                         std::forward<Args>(args)...,
                                                         inner_lerp(get<I>(x0), get<I>(x1), alpha));
                }

                if constexpr(I == D) { // NOLINT fp
                    return EdgeT{std::forward<Args>(args)...};
                }
            }
        };

        template <class EdgeT>
        struct lerp_detail {
            template <class AlphaT>
            static inline constexpr auto op(EdgeT x0, EdgeT x1, AlphaT alpha)
            {
                return inner_lerp(x0, x1, alpha);
            }
        };

        template <class F, class TagT, size_t D>
        struct lerp_detail<abstract_vector<D, F, TagT>> {
            using EdgeT = abstract_vector<D, F, TagT>;

            template <class AlphaT>
            static inline constexpr auto op(EdgeT const &x0, EdgeT const &x1, AlphaT alpha)
            {
                return vec_lerp_detail<D, 0>::op(x0, x1, alpha);
            }
        };
    }

    template <class F, class T>
    constexpr T lerp(T x0, T x1, F alpha)
    {
        return detail::lerp_detail<T>::op(x0, x1, alpha);
    }

    template <class T>
    constexpr auto invlerp(T x0, T x1, T value)
    {
        return (value - x0) / (x1 - x0);
    }

    // Smoothinterp
    namespace detail {
        template <class T>
        inline constexpr T inner_smoothinterp_value(T alpha) noexcept
        {
            auto x = clamp(alpha, make_number_line(T(0), T(1)));
            return x * x * (T(3) - T(2) * x);
        }
    }

    template <class F, class T>
    constexpr T smoothinterp(T x0, T x1, F alpha)
    {
        return lerp(x0, x1, detail::inner_smoothinterp_value(alpha));
    }

    // Smootherinterp
    namespace detail {
        template <class T>
        inline constexpr T inner_smootherinterp_value(T alpha) noexcept
        {
            auto x = clamp(alpha, make_number_line(T(0), T(1)));
            return x * x * x * (x * (x * T(6) - T(15)) + T(10));
        }
    }

    template <class F, class T>
    constexpr T smootherinterp(T x0, T x1, F alpha)
    {
        return lerp(x0, x1, detail::inner_smootherinterp_value(alpha));
    }
}
