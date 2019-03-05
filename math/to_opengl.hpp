#pragma once

#include "abstract_matrix.hpp"

namespace jkgm {
    namespace detail {
        template <class ArrT, class NewF, size_t M, size_t N, size_t I, size_t J>
        struct to_opengl_detail {
            template <class InMatT, class... ArgT>
            static constexpr inline auto op(InMatT const &m, ArgT &&... args) noexcept
            {
                return to_opengl_detail<ArrT, NewF, M, N, I, J + 1>::op(
                    m, std::forward<ArgT>(args)..., static_cast<NewF>(get<I>(get_row<J>(m))));
            }
        };

        template <class ArrT, class NewF, size_t M, size_t N, size_t I>
        struct to_opengl_detail<ArrT, NewF, M, N, I, M> {
            template <class InMatT, class... ArgT>
            static constexpr inline auto op(InMatT const &m, ArgT &&... args) noexcept
            {
                return to_opengl_detail<ArrT, NewF, M, N, I + 1, 0>::op(
                    m, std::forward<ArgT>(args)...);
            }
        };

        template <class ArrT, class NewF, size_t M, size_t N, size_t J>
        struct to_opengl_detail<ArrT, NewF, M, N, N, J> {
            template <class InMatT, class... ArgT>
            static constexpr inline auto op(InMatT const & /*m*/, ArgT &&... args) noexcept
            {
                return ArrT{{std::forward<ArgT>(args)...}};
            }
        };
    }

    template <class NewF, size_t M, size_t N, class F, class Tag>
    constexpr std::array<NewF, M * N> to_opengl(abstract_matrix<M, N, F, Tag> const &m) noexcept
    {
        return detail::to_opengl_detail<std::array<NewF, M * N>, NewF, M, N, 0, 0>::op(m);
    }
}
