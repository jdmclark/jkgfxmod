#pragma once

#include "almost_equal.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include <tuple>
#include <type_traits>

namespace jkgm {
    namespace detail {
        template <class T, size_t I>
        struct fill_detail {
            template <class F, class... ArgT>
            inline static auto constexpr fill(F c, ArgT &&... arg) noexcept
            {
                if constexpr(I > 0) { // NOLINT fp
                    return fill_detail<T, I - 1>::fill(c, std::forward<ArgT>(arg)..., c);
                }

                if constexpr(I == 0) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };

        template <class VecT, class G, size_t N, size_t I>
        struct cast_detail {
            template <class InVecT, class... ArgT>
            inline static constexpr auto op(InVecT const &v, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return cast_detail<VecT, G, N, I + 1>::op(
                        v, std::forward<ArgT>(arg)..., static_cast<G>(std::get<I>(v.data)));
                }

                if constexpr(I == N) { // NOLINT fp
                    return VecT({{std::forward<ArgT>(arg)...}});
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    class abstract_vector {
        static_assert(N > 0, "abstract_vectors must have at least 1 dimension");

    public:
        using field_type = F;
        std::array<F, N> data;

        template <class... G>
        explicit constexpr abstract_vector(G... g) noexcept
            : data({{std::forward<G>(g)...}}) // NOLINT
        {
            static_assert(N == sizeof...(G),
                          "abstract_vector<N> constructor requires N parameters");
        }

        template <class G>
        explicit constexpr abstract_vector(abstract_vector<N, G, Tag> const &v)
            : data(detail::cast_detail<std::array<F, N>, F, N, 0>::op(v))
        {
        }

        static constexpr abstract_vector<N, F, Tag> fill(F c) noexcept
        {
            return detail::fill_detail<abstract_vector<N, F, Tag>, N>::fill(c);
        }

        // EXCL_START can't get test coverage for obligatory constexpr functions
        static constexpr abstract_vector<N, F, Tag> zero() noexcept
        {
            return fill(F(0));
        }
        // EXCL_STOP
    };

    template <size_t I, size_t N, class F, class Tag>
    inline constexpr F &get(abstract_vector<N, F, Tag> &v) noexcept
    {
        static_assert(I < N, "get index out of bounds");
        return std::get<I>(v.data); // NOLINT
    };

    template <size_t I, size_t N, class F, class Tag>
    inline constexpr F get(abstract_vector<N, F, Tag> const &v) noexcept
    {
        static_assert(I < N, "get index out of bounds");
        return std::get<I>(v.data); // NOLINT
    }

    template <class SwizT, size_t N, class F, class Tag>
    inline constexpr auto get(abstract_vector<N, F, Tag> const &v) noexcept
    {
        using namespace jkgm; // NOLINT
        return swizzle(SwizT(), v);
    }

    template <class SwizT,
              size_t N,
              class F,
              class Tag,
              class = std::enable_if_t<SwizT::can_return_reference>>
    inline constexpr auto &get(abstract_vector<N, F, Tag> &v) noexcept
    {
        using namespace jkgm; // NOLINT
        return swizzle(SwizT(), v);
    }

    namespace detail {
        template <class VecT, size_t N, size_t I>
        struct force_retag_detail {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const &v, ArgT &&... arg)
            {
                if constexpr(I < N) { // NOLINT fp
                    return force_retag_detail<VecT, N, I + 1>::op(
                        v, std::forward<ArgT>(arg)..., get<I>(v));
                }

                if constexpr(I == N) { // NOLINT fp
                    return VecT(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <class NewTagT, size_t N, class F, class TagT>
    inline constexpr auto force_retag(abstract_vector<N, F, TagT> v) noexcept
    {
        return detail::force_retag_detail<abstract_vector<N, F, NewTagT>, N, 0>::op(v);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct pairw_eq {
            template <class A, class B>
            static constexpr inline bool op(A const &a, B const &b) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    if(!(get<I>(a) == get<I>(b))) {
                        return false;
                    }

                    return pairw_eq<N, I + 1>::op(a, b);
                }

                return true;
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr bool operator==(abstract_vector<N, F, Tag> a,
                                     abstract_vector<N, F, Tag> b) noexcept
    {
        return detail::pairw_eq<N, 0>::op(a, b);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct pairw_neq {
            template <class A, class B>
            static constexpr inline bool op(A const &a, B const &b) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    if(get<I>(a) != get<I>(b)) {
                        return true;
                    }

                    return pairw_neq<N, I + 1>::op(a, b);
                }

                return false;
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr bool operator!=(abstract_vector<N, F, Tag> a,
                                     abstract_vector<N, F, Tag> b) noexcept
    {
        return detail::pairw_neq<N, 0>::op(a, b);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct pairw_aleq {
            template <class A, class B, class F>
            static constexpr inline bool op(A const &a, B const &b, F maxdiff, F reldiff)
            {
                if constexpr(I < N) { // NOLINT fp
                    if(!almost_equal(get<I>(a), get<I>(b), maxdiff, reldiff)) {
                        return false;
                    }

                    return pairw_aleq<N, I + 1>::op(a, b, maxdiff, reldiff);
                }

                return true;
            }
        };
    }

    template <size_t N, class F, class Tag>
    struct almost_equal_traits<abstract_vector<N, F, Tag>> {
        using field_type = F;
        static constexpr auto default_threshold = almost_equal_traits<F>::default_threshold;
        static constexpr auto default_ulps = almost_equal_traits<F>::default_ulps;

        static inline constexpr bool almost_equal(abstract_vector<N, F, Tag> x,
                                                  abstract_vector<N, F, Tag> y,
                                                  F maxdiff,
                                                  F reldiff)
        {
            return detail::pairw_aleq<N, 0>::op(x, y, maxdiff, reldiff);
        }
    };

    namespace detail {
        template <class Fa, class Fb>
        struct pairw_add_op {
            static constexpr inline auto op(Fa a, Fb b) noexcept
            {
                return a + b;
            }
        };

        template <class Fa, class Fb>
        struct pairw_sub_op {
            static constexpr inline auto op(Fa a, Fb b) noexcept
            {
                return a - b;
            }
        };

        template <class Fa, class Fb>
        struct pairw_mul_op {
            static constexpr inline auto op(Fa a, Fb b) noexcept
            {
                return a * b;
            }
        };

        template <class Fa, class Fb>
        struct pairw_div_op {
            static constexpr inline auto op(Fa a, Fb b) noexcept
            {
                return a / b;
            }
        };

        template <class OpT, class T, size_t N, size_t I>
        struct pairw_op {
            template <class A, class B, class... ArgT>
            static constexpr inline T op(A const &a, B const &b, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return pairw_op<OpT, T, N, I + 1>::op(
                        a, b, std::forward<ArgT>(arg)..., OpT::op(get<I>(a), get<I>(b)));
                }

                if constexpr(I == N) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class TagA, class TagB>
    inline constexpr auto operator+(abstract_vector<N, F, TagA> a,
                                    abstract_vector<N, F, TagB> b) noexcept
    {
        return detail::pairw_op<detail::pairw_add_op<F, F>,
                                abstract_vector<N, F, decltype(TagA() + TagB())>,
                                N,
                                0>::op(a, b);
    }

    template <size_t N, class F, class TagA, class TagB>
    inline constexpr auto operator-(abstract_vector<N, F, TagA> a,
                                    abstract_vector<N, F, TagB> b) noexcept
    {
        return detail::pairw_op<detail::pairw_sub_op<F, F>,
                                abstract_vector<N, F, decltype(TagA() - TagB())>,
                                N,
                                0>::op(a, b);
    }

    template <size_t N, class F, class TagA, class TagB>
    inline constexpr auto operator*(abstract_vector<N, F, TagA> a,
                                    abstract_vector<N, F, TagB> b) noexcept
    {
        return detail::pairw_op<detail::pairw_mul_op<F, F>,
                                abstract_vector<N, F, decltype(TagA() * TagB())>,
                                N,
                                0>::op(a, b);
    }

    template <size_t N, class F, class TagA, class TagB>
    inline constexpr auto operator/(abstract_vector<N, F, TagA> a,
                                    abstract_vector<N, F, TagB> b) noexcept
    {
        return detail::pairw_op<detail::pairw_div_op<F, F>,
                                abstract_vector<N, F, decltype(TagA() / TagB())>,
                                N,
                                0>::op(a, b);
    }

    namespace detail {
        template <class T, size_t N, size_t I>
        struct scal_neg {
            template <class A, class... ArgT>
            static constexpr inline auto op(A const &a, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return scal_neg<T, N, I + 1>::op(a, std::forward<ArgT>(arg)..., -get<I>(a));
                }

                if constexpr(I == N) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto operator-(abstract_vector<N, F, Tag> v) noexcept
    {
        return detail::scal_neg<abstract_vector<N, F, Tag>, N, 0>::op(v);
    }

    namespace detail {
        template <class T, size_t N, size_t I>
        struct scal_mul {
            template <class A, class F, class... ArgT>
            static constexpr inline T op(A const &a, F c, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return scal_mul<T, N, I + 1>::op(
                        a, c, std::forward<ArgT>(arg)..., get<I>(a) * c);
                }

                if constexpr(I == N) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto operator*(abstract_vector<N, F, Tag> a, F c) noexcept
    {
        return detail::scal_mul<abstract_vector<N, F, Tag>, N, 0>::op(a, c);
    }

    namespace detail {
        template <class T, size_t N, size_t I>
        struct scal_premul {
            template <class A, class F, class... ArgT>
            static constexpr inline T op(A const &a, F c, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return scal_premul<T, N, I + 1>::op(
                        a, c, std::forward<ArgT>(arg)..., c * get<I>(a));
                }

                if constexpr(I == N) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto operator*(F c, abstract_vector<N, F, Tag> a) noexcept
    {
        return detail::scal_premul<abstract_vector<N, F, Tag>, N, 0>::op(a, c);
    }

    namespace detail {
        template <class T, size_t N, size_t I>
        struct scal_div {
            template <class A, class F, class... ArgT>
            static constexpr inline T op(A const &a, F c, ArgT &&... arg) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return scal_div<T, N, I + 1>::op(
                        a, c, std::forward<ArgT>(arg)..., get<I>(a) / c);
                }

                if constexpr(I == N) { // NOLINT fp
                    return T(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto operator/(abstract_vector<N, F, Tag> a, F c) noexcept
    {
        return detail::scal_div<abstract_vector<N, F, Tag>, N, 0>::op(a, c);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct pairw_lensq {
            template <class A, class F>
            static constexpr inline auto op(A const &a, F c) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return (get<I>(a) * get<I>(a)) + pairw_lensq<N, I + 1>::op(a, c);
                }

                return c;
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr F length_squared(abstract_vector<N, F, Tag> a) noexcept
    {
        return detail::pairw_lensq<N, 0>::op(a, F(0));
    }

    namespace detail {
        template <size_t N, size_t I>
        struct pairw_dot {
            template <class A, class B, class F>
            static constexpr inline auto op(A const &a, B const &b, F c) noexcept
            {
                if constexpr(I < N) { // NOLINT fp
                    return (get<I>(a) * get<I>(b)) + pairw_dot<N, I + 1>::op(a, b, c);
                }

                return c;
            }
        };
    }

    template <size_t N, class F, class TagA, class TagB>
    inline constexpr F dot(abstract_vector<N, F, TagA> a, abstract_vector<N, F, TagB> b) noexcept
    {
        return detail::pairw_dot<N, 0>::op(a, b, F(0));
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto length(abstract_vector<N, F, Tag> a) noexcept
    {
        return std::sqrt(length_squared(a));
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto normalize(abstract_vector<N, F, Tag> a) noexcept
    {
        return a / length(a);
    }

    template <class F, class Tag>
    inline constexpr auto cross(abstract_vector<3, F, Tag> v, abstract_vector<3, F, Tag> w) noexcept
    {
        return abstract_vector<3, F, Tag>(get<1>(v) * get<2>(w) - get<2>(v) * get<1>(w),
                                          get<2>(v) * get<0>(w) - get<0>(v) * get<2>(w),
                                          get<0>(v) * get<1>(w) - get<1>(v) * get<0>(w));
    }

    namespace detail {
        template <class VecT, size_t I>
        struct extend_detail {
            template <class InVecT, class... ArgT>
            static constexpr inline auto op(InVecT const &v, ArgT &&... arg)
            {
                if constexpr(I > 0) { // NOLINT fp
                    return extend_detail<VecT, I - 1>::op(
                        v, get<I - 1>(v), std::forward<ArgT>(arg)...);
                }

                if constexpr(I == 0) { // NOLINT fp
                    return VecT(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag, class... ArgT>
    inline constexpr auto extend(abstract_vector<N, F, Tag> v, ArgT &&... c)
    {
        return detail::extend_detail<abstract_vector<N + sizeof...(ArgT), F, Tag>, N>::op(
            v, std::forward<ArgT>(c)...);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct vector_pairwise_min_detail {
            template <class VecT, class... ArgT>
            static constexpr inline auto op(VecT const &v0, VecT const &v1, ArgT &&... arg)
            {
                if constexpr(I < N) { // NOLINT fp
                    return vector_pairwise_min_detail<N, I + 1>::op(
                        v0, v1, std::forward<ArgT>(arg)..., std::min(get<I>(v0), get<I>(v1)));
                }

                if constexpr(I == N) { // NOLINT fp
                    return VecT(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto pairwise_min(abstract_vector<N, F, Tag> v0, abstract_vector<N, F, Tag> v1)
    {
        return detail::vector_pairwise_min_detail<N, 0>::op(v0, v1);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct vector_pairwise_max_detail {
            template <class VecT, class... ArgT>
            static constexpr inline auto op(VecT const &v0, VecT const &v1, ArgT &&... arg)
            {
                if constexpr(I < N) { // NOLINT fp
                    return vector_pairwise_max_detail<N, I + 1>::op(
                        v0, v1, std::forward<ArgT>(arg)..., std::max(get<I>(v0), get<I>(v1)));
                }

                if constexpr(I == N) { // NOLINT fp
                    return VecT(std::forward<ArgT>(arg)...);
                }
            }
        };
    }

    template <size_t N, class F, class Tag>
    inline constexpr auto pairwise_max(abstract_vector<N, F, Tag> v0, abstract_vector<N, F, Tag> v1)
    {
        return detail::vector_pairwise_max_detail<N, 0>::op(v0, v1);
    }
}

namespace std {
    template <size_t N, class F, class Tag>
    struct tuple_size<::jkgm::abstract_vector<N, F, Tag>>
        : public std::integral_constant<std::size_t, N> {
    };

    template <size_t I, size_t N, class F, class Tag>
    struct tuple_element<I, ::jkgm::abstract_vector<N, F, Tag>> {
        using type = F;
    };
}
