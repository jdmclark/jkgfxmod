#pragma once

#include "direction.hpp"
#include "point.hpp"
#include "size.hpp"

namespace jkgm {
    namespace detail {
        template <size_t N, size_t I>
        struct box_contains_detail {
            template <class B, class V>
            inline static constexpr bool op(B const &b, V const &v) noexcept
            {
                if(get<I>(b.start) > get<I>(v) || get<I>(b.stop) < get<I>(v)) {
                    return false;
                }

                return box_contains_detail<N, I + 1>::op(b, v);
            }
        };

        template <size_t N>
        struct box_contains_detail<N, N> {
            template <class B, class V>
            inline static constexpr bool op(B const & /*b*/, V const & /*v*/) noexcept
            {
                return true;
            }
        };
    }

    template <size_t N, class F>
    class box {
    public:
        using point_type = point<N, F>;
        using size_type = jkgm::size<N, F>;

        point<N, F> start;
        point<N, F> stop;

        constexpr box(point<N, F> start, point<N, F> stop) noexcept
            : start(start)
            , stop(stop)
        {
        }

        constexpr box(point<N, F> start, size<N, F> dims) noexcept
            : start(start)
            , stop(start + force_retag<direction_vector_tag>(dims))
        {
        }

        constexpr box(F start, F stop) noexcept
            : start(start)
            , stop(stop)
        {
        }

        template <class G>
        explicit constexpr box(box<N, G> const &b)
            : start(static_cast<point<N, F>>(b.start))
            , stop(static_cast<point<N, F>>(b.stop))
        {
        }

        constexpr auto size() const noexcept
        {
            return force_retag<size_vector_tag>(stop - start);
        }

        constexpr bool contains(point<N, F> const &v) const noexcept
        {
            return detail::box_contains_detail<N, 0>::op(*this, v);
        }

        constexpr bool contains(F c) const noexcept
        {
            static_assert(N == 1, "bare scalar box::contains overload used on k-d box");
            return get<0>(start) <= c && get<0>(stop) >= c;
        }

        constexpr bool contains(box<N, F> const &v) const noexcept
        {
            return contains(v.start) && contains(v.stop);
        }
    };

    template <class F>
    using number_line = box<1, F>;

    template <class F>
    constexpr auto make_number_line(F first, F second)
    {
        return number_line<F>(first, second);
    }

    template <size_t N, class F>
    constexpr auto make_box(point<N, F> topleft, point<N, F> bottomright)
    {
        return box<N, F>(topleft, bottomright);
    }

    template <size_t N, class F>
    constexpr auto make_box(point<N, F> origin, size<N, F> sz)
    {
        return box<N, F>(origin, sz);
    }

    template <size_t N, class F>
    constexpr bool operator==(box<N, F> const &a, box<N, F> const &b) noexcept
    {
        return (a.start == b.start) && (a.stop == b.stop);
    }

    template <size_t N, class F>
    constexpr bool operator!=(box<N, F> const &a, box<N, F> const &b) noexcept
    {
        return (a.start != b.start) || (a.stop != b.stop);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct intersect_detail {
            template <class BoxT, class... EmT>
            static inline constexpr auto op(BoxT const &a, BoxT const &b, EmT &&... em) noexcept
            {
                auto a_start = get<I>(a.start);
                auto a_stop = get<I>(a.stop);
                auto b_start = get<I>(b.start);
                auto b_stop = get<I>(b.stop);

                if(a_start >= b_stop || b_start >= a_stop) {
                    // Ranges are disjoint
                    return intersect_detail<N, I + 1>::op(
                        a, b, std::forward<EmT>(em)..., std::make_pair(a_start, a_start));
                }

                return intersect_detail<N, I + 1>::op(a,
                                                      b,
                                                      std::forward<EmT>(em)...,
                                                      std::make_pair(std::max(a_start, b_start),
                                                                     std::min(a_stop, b_stop)));
            }
        };

        template <size_t N>
        struct intersect_detail<N, N> {
            template <class BoxT, class... EmT>
            static inline constexpr auto
                op(BoxT const & /*a*/, BoxT const & /*b*/, EmT &&... em) noexcept
            {
                return BoxT(typename BoxT::point_type(get<0>(em)...),
                            typename BoxT::point_type(get<1>(em)...));
            }
        };
    }

    template <size_t N, class F>
    constexpr auto intersect(box<N, F> const &a, box<N, F> const &b) noexcept
    {
        return detail::intersect_detail<N, 0>::op(a, b);
    }

    template <size_t N, class F>
    constexpr auto volume(box<N, F> const &a) noexcept
    {
        return volume(a.size());
    }

    template <size_t N, class F>
    constexpr auto extend(box<N, F> const &a, point<N, F> const &b)
    {
        return make_box(pairwise_min(a.start, b), pairwise_max(a.stop, b));
    }

    template <size_t N, class F>
    constexpr auto extend(box<N, F> const &a, box<N, F> const &b)
    {
        return make_box(pairwise_min(a.start, b.start), pairwise_max(a.stop, b.stop));
    }
}
