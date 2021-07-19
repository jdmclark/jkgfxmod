#pragma once

#include "gl.hpp"
#include "shader.hpp"
#include "base/cstring_view.hpp"
#include "base/id.hpp"
#include "base/unique_handle.hpp"
#include <optional>

namespace jkgm::gl {
    struct program_traits {
        using value_type = uint_type;

        static uint_type create();
        static void destroy(uint_type id);
    };

    using program = unique_handle<program_traits>;
    using program_view = unique_handle_view<program_traits>;

    MAKE_ID_TYPE(uniform_location, int_type);

    void attach_shader(program_view id, shader_view s);
    void link_program(program_view id);

    bool get_program_link_status(program_view id);
    std::string get_program_info_log(program_view id);

    void use_program(program_view prog);
    uniform_location_id get_uniform_location(program_view prog, cstring_view name);

    namespace detail {
        void inner_set_uniform_matrix_4fv(uniform_location_id uniform,
                                          bool transpose,
                                          std::array<float, 16> const &value);

        void inner_set_uniform_vec(uniform_location_id uniform, int v0);
        void inner_set_uniform_vec(uniform_location_id uniform, int v0, int v1);
        void inner_set_uniform_vec(uniform_location_id uniform, int v0, int v1, int v2);
        void inner_set_uniform_vec(uniform_location_id uniform, int v0, int v1, int v2, int v3);

        void inner_set_uniform_vec(uniform_location_id uniform, float v0);
        void inner_set_uniform_vec(uniform_location_id uniform, float v0, float v1);
        void inner_set_uniform_vec(uniform_location_id uniform, float v0, float v1, float v2);
        void inner_set_uniform_vec(uniform_location_id uniform,
                                   float v0,
                                   float v1,
                                   float v2,
                                   float v3);

        template <size_t N, size_t I>
        struct inner_set_uniform_vec_apply {
            template <class VecT, class... ArgT>
            static inline constexpr void op(VecT const &v, ArgT &&... args)
            {
                inner_set_uniform_vec_apply<N, I + 1>::op(
                    v, std::forward<ArgT>(args)..., get<I>(v));
            }
        };

        template <size_t N>
        struct inner_set_uniform_vec_apply<N, N> {
            template <class VecT, class... ArgT>
            static inline constexpr void op(VecT const & /*v*/, ArgT &&... args)
            {
                inner_set_uniform_vec(std::forward<ArgT>(args)...);
            }
        };
    }

    void set_uniform_integer(uniform_location_id uniform, int value);
    void set_uniform_float(uniform_location_id uniform, float value);

    template <size_t N, class F, class TagT>
    void set_uniform_vector(uniform_location_id uniform, abstract_vector<N, F, TagT> const &v)
    {
        detail::inner_set_uniform_vec_apply<N, 0>::op(v, uniform);
    }
}
