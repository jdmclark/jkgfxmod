#pragma once

#include "abstract_vector.hpp"

namespace jkgm {
    struct matrix_row_vector_tag {
    };

    struct matrix_diag_vector_tag {
    };

    // Operators
    matrix_row_vector_tag operator+(matrix_row_vector_tag, matrix_row_vector_tag);
    matrix_row_vector_tag operator-(matrix_row_vector_tag, matrix_row_vector_tag);

    namespace detail {
        template <class TagT, class F0, class... FN>
        inline constexpr auto mkmatvec(F0 &&f0, FN &&... fn) noexcept
        {
            return abstract_vector<sizeof...(FN) + 1, std::decay_t<F0>, TagT>(
                std::forward<F0>(f0), std::forward<FN>(fn)...);
        }
    }

    template <class F0, class... FN>
    inline constexpr auto row(F0 &&f0, FN &&... fn) noexcept
    {
        return detail::mkmatvec<matrix_row_vector_tag, F0, FN...>(std::forward<F0>(f0),
                                                                  std::forward<FN>(fn)...);
    }

    template <class F0, class... FN>
    inline constexpr auto diagonal(F0 &&f0, FN &&... fn) noexcept
    {
        return detail::mkmatvec<matrix_diag_vector_tag, F0, FN...>(std::forward<F0>(f0),
                                                                   std::forward<FN>(fn)...);
    }

    namespace detail {
        template <class MatT, size_t RowWidth, size_t CurrRow, size_t CurrIdx>
        struct mat_diag_row_detail {
            template <class F, class... EmT>
            inline constexpr static auto op(F kd_val, EmT &&... em) noexcept
            {
                return mat_diag_row_detail<MatT, RowWidth, CurrRow, CurrIdx + 1>::op(
                    kd_val, std::forward<EmT>(em)..., F(0));
            }
        };

        template <class MatT, size_t RowWidth, size_t CurrRow>
        struct mat_diag_row_detail<MatT, RowWidth, CurrRow, CurrRow> {
            template <class F, class... EmT>
            inline constexpr static auto op(F kd_val, EmT &&... em) noexcept
            {
                return mat_diag_row_detail<MatT, RowWidth, CurrRow, CurrRow + 1>::op(
                    kd_val, std::forward<EmT>(em)..., kd_val);
            }
        };

        template <class MatT, size_t RowWidth, size_t CurrRow>
        struct mat_diag_row_detail<MatT, RowWidth, CurrRow, RowWidth> {
            template <class F, class... EmT>
            inline constexpr static auto op(F /*kd_val*/, EmT &&... em) noexcept
            {
                return typename MatT::row_vector(std::forward<EmT>(em)...);
            }
        };

        template <class MatT, size_t RowWidth>
        struct mat_diag_row_detail<MatT, RowWidth, RowWidth, RowWidth> {
            template <class F, class... EmT>
            inline constexpr static auto op(F /*kd_val*/, EmT &&... em) noexcept
            {
                return typename MatT::row_vector(std::forward<EmT>(em)...);
            }
        };

        template <class MatT, size_t RowWidth, size_t NumRows, size_t DiagLeft, size_t CurrRow>
        struct mat_diag_detail_internal {
            template <class DiagT, class... EmT>
            inline constexpr static auto op(DiagT const &diag, EmT &&... em) noexcept
            {
                return mat_diag_detail_internal<
                    MatT,
                    RowWidth,
                    NumRows,
                    DiagLeft - 1,
                    CurrRow + 1>::op(diag,
                                     std::forward<EmT>(em)...,
                                     mat_diag_row_detail<MatT, RowWidth, CurrRow, 0>::op(
                                         get<CurrRow>(diag)));
            }
        };

        template <class MatT, size_t RowWidth, size_t NumRows, size_t CurrRow>
        struct mat_diag_detail_internal<MatT, RowWidth, NumRows, 0, CurrRow> {
            template <class DiagT, class... EmT>
            inline constexpr static auto op(DiagT const &diag, EmT &&... em) noexcept
            {
                return mat_diag_detail_internal<MatT, RowWidth, NumRows, 0, CurrRow + 1>::op(
                    diag,
                    std::forward<EmT>(em)...,
                    mat_diag_row_detail<MatT, RowWidth, CurrRow, 0>::op(decltype(get<0>(diag))(0)));
            }
        };

        template <class MatT, size_t RowWidth, size_t NumRows>
        struct mat_diag_detail_internal<MatT, RowWidth, NumRows, 0, NumRows> {
            template <class DiagT, class... EmT>
            inline constexpr static auto op(DiagT const & /*diag*/, EmT &&... em) noexcept
            {
                return MatT(std::forward<EmT>(em)...);
            }
        };

        template <class MatT, size_t RowWidth, size_t NumRows>
        struct mat_diag_detail {
            template <class DiagT>
            inline constexpr static auto op(DiagT const &diag) noexcept
            {
                return mat_diag_detail_internal<MatT,
                                                RowWidth,
                                                NumRows,
                                                std::min(RowWidth, NumRows),
                                                0>::op(diag);
            }
        };
    }

    template <size_t M, size_t N, class F, class Tag>
    class abstract_matrix {
        static_assert(M > 0, "abstract_matrices must have at least 1 row");
        static_assert(N > 0, "abstract_matrices must have at least 1 col");

    public:
        using row_vector = abstract_vector<N, F, matrix_row_vector_tag>;
        using diag_vector = abstract_vector<std::min(M, N), F, matrix_diag_vector_tag>;

        std::array<row_vector, M> data;

        template <class... G>
        explicit constexpr abstract_matrix(G... g) noexcept
            : data({{std::forward<G>(g)...}})
        {
            static_assert(M == sizeof...(G),
                          "abstract_matrix<M, N> constructor requires M parameters");
        }

        static abstract_matrix<M, N, F, Tag> diagonal(diag_vector dv) noexcept
        {
            return detail::mat_diag_detail<abstract_matrix<M, N, F, Tag>, N, M>::op(dv);
        }

        static abstract_matrix<M, N, F, Tag> identity() noexcept
        {
            static_assert(M == N,
                          "abstract_matrix<M, N>::identity is only valid for square matrices");
            return diagonal(diag_vector::fill(F(1)));
        }
    };

    template <size_t I, size_t M, size_t N, class F, class Tag>
    constexpr auto const &get_row(abstract_matrix<M, N, F, Tag> const &m) noexcept
    {
        static_assert(I < M, "get_row index out of bounds");
        return std::get<I>(m.data);
    }

    namespace detail {
        template <class RowT, size_t CurrInCol, size_t NumInRows, size_t CurrInRow>
        struct mat_transpose_row_detail {
            template <class InMatT, class... ArgT>
            static inline constexpr RowT op(InMatT const &m, ArgT &&... args) noexcept
            {
                return mat_transpose_row_detail<RowT, CurrInCol, NumInRows, CurrInRow + 1>::op(
                    m, std::forward<ArgT>(args)..., get<CurrInCol>(get_row<CurrInRow>(m)));
            }
        };

        template <class RowT, size_t CurrInCol, size_t NumInRows>
        struct mat_transpose_row_detail<RowT, CurrInCol, NumInRows, NumInRows> {
            template <class InMatT, class... ArgT>
            static inline constexpr RowT op(InMatT const & /*m*/, ArgT &&... args) noexcept
            {
                return RowT(std::forward<ArgT>(args)...);
            }
        };

        template <class MatT, size_t NumOutRows, size_t NumOutCols, size_t CurrOutRow>
        struct mat_transpose_detail {
            template <class InMatT, class... ArgT>
            static inline constexpr MatT op(InMatT const &m, ArgT &&... args) noexcept
            {
                return mat_transpose_detail<MatT, NumOutRows, NumOutCols, CurrOutRow + 1>::op(
                    m,
                    std::forward<ArgT>(args)...,
                    mat_transpose_row_detail<typename MatT::row_vector, CurrOutRow, NumOutCols, 0>::
                        op(m));
            }
        };

        template <class MatT, size_t NumOutRows, size_t NumOutCols>
        struct mat_transpose_detail<MatT, NumOutRows, NumOutCols, NumOutRows> {
            template <class InMatT, class... ArgT>
            static inline constexpr MatT op(InMatT const & /*m*/, ArgT &&... args) noexcept
            {
                return MatT(std::forward<ArgT>(args)...);
            }
        };
    }

    template <size_t M, size_t N, class F, class Tag>
    constexpr abstract_matrix<N, M, F, Tag>
        transpose(abstract_matrix<M, N, F, Tag> const &m) noexcept
    {
        return detail::mat_transpose_detail<abstract_matrix<N, M, F, Tag>, N, M, 0>::op(m);
    }

    namespace detail {
        template <size_t NumRows, size_t CurrRow>
        struct mat_pairw_eq {
            template <class A, class B>
            static constexpr inline bool op(A const &a, B const &b) noexcept
            {
                if(!(get_row<CurrRow>(a) == get_row<CurrRow>(b))) {
                    return false;
                }

                return mat_pairw_eq<NumRows, CurrRow + 1>::op(a, b);
            }
        };

        template <size_t NumRows>
        struct mat_pairw_eq<NumRows, NumRows> {
            template <class A, class B>
            static constexpr inline bool op(A const & /*a*/, B const & /*b*/) noexcept
            {
                return true;
            }
        };
    }

    template <size_t M, size_t N, class F, class Tag>
    constexpr bool operator==(abstract_matrix<M, N, F, Tag> const &a,
                              abstract_matrix<M, N, F, Tag> const &b) noexcept
    {
        return detail::mat_pairw_eq<M, 0>::op(a, b);
    }

    namespace detail {
        template <size_t NumRows, size_t CurrRow>
        struct mat_pairw_neq {
            template <class A, class B>
            static constexpr inline bool op(A const &a, B const &b) noexcept
            {
                if(get_row<CurrRow>(a) != get_row<CurrRow>(b)) {
                    return true;
                }

                return mat_pairw_neq<NumRows, CurrRow + 1>::op(a, b);
            }
        };

        template <size_t NumRows>
        struct mat_pairw_neq<NumRows, NumRows> {
            template <class A, class B>
            static constexpr inline bool op(A const & /*a*/, B const & /*b*/) noexcept
            {
                return false;
            }
        };
    }

    template <size_t M, size_t N, class F, class Tag>
    constexpr bool operator!=(abstract_matrix<M, N, F, Tag> const &a,
                              abstract_matrix<M, N, F, Tag> const &b) noexcept
    {
        return detail::mat_pairw_neq<M, 0>::op(a, b);
    }

    namespace detail {
        template <size_t N, size_t I>
        struct mat_pairw_aleq {
            template <class A, class B, class F>
            static constexpr inline bool op(A const &a, B const &b, F maxdiff, F reldiff) noexcept
            {
                if(!almost_equal(get_row<I>(a), get_row<I>(b), maxdiff, reldiff)) {
                    return false;
                }

                return mat_pairw_aleq<N, I + 1>::op(a, b, maxdiff, reldiff);
            }
        };

        template <size_t N>
        struct mat_pairw_aleq<N, N> {
            template <class A, class B, class F>
            static constexpr inline bool
                op(A const & /*a*/, B const & /*b*/, F /*maxdiff*/, F /*reldiff*/) noexcept
            {
                return true;
            }
        };
    }

    template <size_t M, size_t N, class F, class Tag>
    struct almost_equal_traits<abstract_matrix<M, N, F, Tag>> {
        using field_type = F;
        static constexpr auto default_threshold = almost_equal_traits<F>::default_threshold;
        static constexpr auto default_ulps = almost_equal_traits<F>::default_ulps;

        static inline constexpr bool almost_equal(abstract_matrix<M, N, F, Tag> const &x,
                                                  abstract_matrix<M, N, F, Tag> const &y,
                                                  F maxdiff,
                                                  F reldiff)
        {
            return detail::mat_pairw_aleq<M, 0>::op(x, y, maxdiff, reldiff);
        }
    };

    namespace detail {
        template <class A, class B>
        struct mat_pairw_add_op {
            static constexpr inline auto op(A const &a, B const &b) noexcept
            {
                return a + b;
            }
        };

        template <class A, class B>
        struct mat_pairw_sub_op {
            static constexpr inline auto op(A const &a, B const &b) noexcept
            {
                return a - b;
            }
        };

        template <class OpT, class T, size_t RowCount, size_t CurrRow>
        struct mat_pairw_op {
            template <class A, class B, class... ArgT>
            static constexpr inline T op(A const &a, B const &b, ArgT &&... arg) noexcept
            {
                return mat_pairw_op<OpT, T, RowCount, CurrRow + 1>::op(
                    a,
                    b,
                    std::forward<ArgT>(arg)...,
                    OpT::op(get_row<CurrRow>(a), get_row<CurrRow>(b)));
            }
        };

        template <class OpT, class T, size_t RowCount>
        struct mat_pairw_op<OpT, T, RowCount, RowCount> {
            template <class A, class B, class... ArgT>
            static constexpr inline T op(A const & /*a*/, B const & /*b*/, ArgT &&... arg) noexcept
            {
                return T(std::forward<ArgT>(arg)...);
            }
        };
    }

    template <size_t M, size_t N, class F, class TagA, class TagB>
    constexpr auto operator+(abstract_matrix<M, N, F, TagA> const &a,
                             abstract_matrix<M, N, F, TagB> const &b) noexcept
    {
        using row_vector = abstract_vector<N, F, matrix_row_vector_tag>;
        return detail::mat_pairw_op<detail::mat_pairw_add_op<row_vector, row_vector>,
                                    abstract_matrix<M, N, F, decltype(TagA() + TagB())>,
                                    M,
                                    0>::op(a, b);
    }

    template <size_t M, size_t N, class F, class TagA, class TagB>
    constexpr auto operator-(abstract_matrix<M, N, F, TagA> const &a,
                             abstract_matrix<M, N, F, TagB> const &b) noexcept
    {
        using row_vector = abstract_vector<N, F, matrix_row_vector_tag>;
        return detail::mat_pairw_op<detail::mat_pairw_sub_op<row_vector, row_vector>,
                                    abstract_matrix<M, N, F, decltype(TagA() + TagB())>,
                                    M,
                                    0>::op(a, b);
    }

    namespace detail {
        template <size_t N, size_t CurrStep, size_t CurrPart>
        struct mat_mul_comp_cell {
            template <class A, class B, class... ArgT>
            static inline constexpr auto op(A const &a, B const &b, ArgT &&... args) noexcept
            {
                return mat_mul_comp_cell<N, CurrStep, CurrPart + 1>::op(
                    a,
                    b,
                    std::forward<ArgT>(args)...,
                    get<CurrPart>(a) * get<CurrStep>(get_row<CurrPart>(b)));
            }
        };

        template <size_t N, size_t CurrStep>
        struct mat_mul_comp_cell<N, CurrStep, N> {
            template <class A, class B, class... ArgT>
            static inline constexpr auto
                op(A const & /*a*/, B const & /*b*/, ArgT &&... args) noexcept
            {
                return (args + ...);
            }
        };

        template <class RowT, size_t N, size_t O, size_t CurrOutRow, size_t CurrStep>
        struct mat_mul_comp_row {
            template <class A, class B, class... ArgT>
            static inline constexpr auto op(A const &a, B const &b, ArgT &&... args) noexcept
            {
                return mat_mul_comp_row<RowT, N, O, CurrOutRow, CurrStep + 1>::op(
                    a,
                    b,
                    std::forward<ArgT>(args)...,
                    mat_mul_comp_cell<N, CurrStep, 0>::op(get_row<CurrOutRow>(a), b));
            }
        };

        template <class RowT, size_t N, size_t O, size_t CurrOutRow>
        struct mat_mul_comp_row<RowT, N, O, CurrOutRow, O> {
            template <class A, class B, class... ArgT>
            static inline constexpr auto
                op(A const & /*a*/, B const & /*b*/, ArgT &&... args) noexcept
            {
                return RowT(std::forward<ArgT>(args)...);
            }
        };

        template <class MatT, size_t M, size_t N, size_t O, size_t CurrOutRow>
        struct mat_mul_detail {
            template <class A, class B, class... ArgT>
            static inline constexpr auto op(A const &a, B const &b, ArgT &&... args) noexcept
            {
                return mat_mul_detail<MatT, M, N, O, CurrOutRow + 1>::op(
                    a,
                    b,
                    std::forward<ArgT>(args)...,
                    mat_mul_comp_row<typename MatT::row_vector, N, O, CurrOutRow, 0>::op(a, b));
            }
        };

        template <class MatT, size_t M, size_t N, size_t O>
        struct mat_mul_detail<MatT, M, N, O, M> {
            template <class A, class B, class... ArgT>
            static inline constexpr auto
                op(A const & /*a*/, B const & /*b*/, ArgT &&... args) noexcept
            {
                return MatT(std::forward<ArgT>(args)...);
            }
        };
    }

    template <size_t M, size_t N, size_t O, size_t P, class F, class TagA, class TagB>
    constexpr auto operator*(abstract_matrix<M, N, F, TagA> const &a,
                             abstract_matrix<O, P, F, TagB> const &b) noexcept
    {
        static_assert(N == O, "matrix sizes are not valid for multiplication");
        return detail::
            mat_mul_detail<abstract_matrix<M, P, F, decltype(TagA() * TagB())>, M, N, P, 0>::op(a,
                                                                                                b);
    }

    namespace detail {
        template <size_t N, size_t CurrCol>
        struct mat_vec_comp_cell {
            template <class A, class B, class... ArgT>
            static inline constexpr auto op(A const &a, B const &b, ArgT &&... args) noexcept
            {
                return mat_vec_comp_cell<N, CurrCol + 1>::op(
                    a, b, std::forward<ArgT>(args)..., get<CurrCol>(a) * get<CurrCol>(b));
            }
        };

        template <size_t N>
        struct mat_vec_comp_cell<N, N> {
            template <class A, class B, class... ArgT>
            static inline constexpr auto
                op(A const & /*a*/, B const & /*b*/, ArgT &&... args) noexcept
            {
                return (args + ...);
            }
        };

        template <class VecT, size_t M, size_t N, size_t CurrRow>
        struct mat_vec_mul_detail {
            template <class A, class B, class... ArgT>
            static inline constexpr auto op(A const &a, B const &b, ArgT &&... args) noexcept
            {
                return mat_vec_mul_detail<VecT, M, N, CurrRow + 1>::op(
                    a,
                    b,
                    std::forward<ArgT>(args)...,
                    mat_vec_comp_cell<N, 0>::op(get_row<CurrRow>(a), b));
            }
        };

        template <class VecT, size_t M, size_t N>
        struct mat_vec_mul_detail<VecT, M, N, M> {
            template <class A, class B, class... ArgT>
            static inline constexpr auto
                op(A const & /*a*/, B const & /*b*/, ArgT &&... args) noexcept
            {
                return VecT(std::forward<ArgT>(args)...);
            }
        };
    }

    template <size_t M, size_t N, size_t O, class F, class TagA, class TagB>
    constexpr auto operator*(abstract_matrix<M, N, F, TagA> const &a,
                             abstract_vector<O, F, TagB> const &v) noexcept
    {
        static_assert(N == O, "vector size insufficient for multiplication");
        return detail::
            mat_vec_mul_detail<abstract_vector<M, F, decltype(TagA() * TagB())>, M, N, 0>::op(a, v);
    }

    namespace detail {
        template <class MatT, size_t NumRows, size_t CurrRow>
        struct mat_pre_scale_detail {
            template <class InMatT, class F, class... ArgT>
            static inline constexpr auto op(InMatT const &a, F c, ArgT &&... args) noexcept
            {
                return mat_pre_scale_detail<MatT, NumRows, CurrRow + 1>::op(
                    a, c, std::forward<ArgT>(args)..., c * get_row<CurrRow>(a));
            }
        };

        template <class MatT, size_t NumRows>
        struct mat_pre_scale_detail<MatT, NumRows, NumRows> {
            template <class InMatT, class F, class... ArgT>
            static inline constexpr auto op(InMatT const & /*a*/, F /*c*/, ArgT &&... args) noexcept
            {
                return MatT(std::forward<ArgT>(args)...);
            }
        };

        template <class MatT, size_t NumRows, size_t CurrRow>
        struct mat_post_scale_detail {
            template <class InMatT, class F, class... ArgT>
            static inline constexpr auto op(InMatT const &a, F c, ArgT &&... args) noexcept
            {
                return mat_post_scale_detail<MatT, NumRows, CurrRow + 1>::op(
                    a, c, std::forward<ArgT>(args)..., get_row<CurrRow>(a) * c);
            }
        };

        template <class MatT, size_t NumRows>
        struct mat_post_scale_detail<MatT, NumRows, NumRows> {
            template <class InMatT, class F, class... ArgT>
            static inline constexpr auto op(InMatT const & /*a*/, F /*c*/, ArgT &&... args) noexcept
            {
                return MatT(std::forward<ArgT>(args)...);
            }
        };

        template <class OutTag, size_t M, size_t N, class F, class InTag>
        inline constexpr auto mat_pre_scale(abstract_matrix<M, N, F, InTag> const &m, F c) noexcept
        {
            return mat_pre_scale_detail<abstract_matrix<M, N, F, OutTag>, M, 0>::op(m, c);
        }

        template <class OutTag, size_t M, size_t N, class F, class InTag>
        inline constexpr auto mat_post_scale(abstract_matrix<M, N, F, InTag> const &m, F c) noexcept
        {
            return mat_post_scale_detail<abstract_matrix<M, N, F, OutTag>, M, 0>::op(m, c);
        }
    }

    template <size_t M, size_t N, class F, class Tag>
    constexpr auto operator*(F c, abstract_matrix<M, N, F, Tag> const &m) noexcept
    {
        return detail::mat_pre_scale<Tag>(m, c);
    }

    template <size_t M, size_t N, class F, class Tag>
    constexpr auto operator*(abstract_matrix<M, N, F, Tag> const &m, F c) noexcept
    {
        return detail::mat_post_scale<Tag>(m, c);
    }

    template <size_t M, size_t N, class F, class TagA, class TagB>
    constexpr auto operator*(abstract_scalar<F, TagB> c,
                             abstract_matrix<M, N, F, TagA> const &m) noexcept
    {
        return detail::mat_pre_scale<decltype(TagB() * TagA())>(m, get(c));
    }

    template <size_t M, size_t N, class F, class TagA, class TagB>
    constexpr auto operator*(abstract_matrix<M, N, F, TagA> const &m,
                             abstract_scalar<F, TagB> c) noexcept
    {
        return detail::mat_post_scale<decltype(TagA() * TagB())>(m, get(c));
    }

    namespace detail {
        template <class VecT, size_t N, size_t SkipCol, size_t I>
        struct mat_det_minor_row {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const &v, ArgT &&... args) noexcept
            {
                if constexpr(I == SkipCol) {
                    return mat_det_minor_row<VecT, N, SkipCol, I + 1>::op(
                        v, std::forward<ArgT>(args)...);
                }

                if constexpr(I != SkipCol) {
                    return mat_det_minor_row<VecT, N, SkipCol, I + 1>::op(
                        v, std::forward<ArgT>(args)..., get<I>(v));
                }
            }
        };

        template <class VecT, size_t N, size_t SkipCol>
        struct mat_det_minor_row<VecT, N, SkipCol, N> {
            template <class InVecT, class... ArgT>
            static inline constexpr auto op(InVecT const & /*v*/, ArgT &&... args) noexcept
            {
                return VecT(std::forward<ArgT>(args)...);
            }
        };

        template <class MatT, size_t N, size_t SkipCol, size_t I>
        struct mat_det_minor {
            template <class InMatT, class... RowT>
            static inline constexpr auto op(InMatT const &m, RowT &&... rows) noexcept
            {
                return mat_det_minor<MatT, N, SkipCol, I + 1>::op(
                    m,
                    std::forward<RowT>(rows)...,
                    mat_det_minor_row<typename MatT::row_vector, N, SkipCol, 0>::op(get_row<I>(m)));
            }
        };

        template <class MatT, size_t N, size_t SkipCol>
        struct mat_det_minor<MatT, N, SkipCol, N> {
            template <class InMatT, class... RowT>
            static inline constexpr auto op(InMatT const & /*m*/, RowT &&... rows) noexcept
            {
                return MatT(std::forward<RowT>(rows)...);
            }
        };

        template <size_t N, size_t I>
        struct mat_det {
            template <class F, class Tag>
            static inline constexpr auto op(abstract_matrix<N, N, F, Tag> const &m) noexcept
            {
                using MinorT = abstract_matrix<N - 1, N - 1, F, Tag>;
                auto rv1 = get<I>(get_row<0>(m)) *
                           mat_det<N - 1, 0>::op(mat_det_minor<MinorT, N, I, 1>::op(m));
                auto rv2 = mat_det<N, I + 1>::op(m);

                if constexpr((I % 2) == 0) {
                    return rv1 + rv2;
                }

                if constexpr((I % 2) != 0) {
                    return -rv1 + rv2;
                }
            }
        };

        template <size_t N>
        struct mat_det<N, N> {
            template <class F, class Tag>
            static inline constexpr auto op(abstract_matrix<N, N, F, Tag> const & /*m*/) noexcept
            {
                return F(0);
            }
        };

        template <>
        struct mat_det<1, 0> {
            template <class MatT>
            static inline constexpr auto op(MatT const &m) noexcept
            {
                return get<0>(get_row<0>(m));
            }
        };
    }

    template <size_t N, class F, class Tag>
    constexpr auto determinant(abstract_matrix<N, N, F, Tag> const &m) noexcept
    {
        return detail::mat_det<N, 0>::op(m);
    }
}
