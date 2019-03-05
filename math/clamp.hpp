#pragma once

#include "box.hpp"

namespace jkgm {
    namespace detail {
        template <class F>
        inline constexpr F inner_clamp(F &&value, F &&min, F &&max) noexcept
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
                return inner_clamp(std::forward<T>(value), get<0>(b.start), get<0>(b.stop));
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
}
