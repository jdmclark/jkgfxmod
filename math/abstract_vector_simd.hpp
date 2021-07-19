#pragma once

#include "abstract_vector_def.hpp"
#include <array>
#include <immintrin.h>

namespace jkgm {
    template <class Tag>
    class abstract_vector<4, float, Tag> {
    public:
        using field_type = float;
        alignas(16) std::array<float, 4> data;

        constexpr abstract_vector(float x, float y, float z, float w) noexcept
            : data({{x, y, z, w}})
        {
        }

        template <class G>
        explicit constexpr abstract_vector(abstract_vector<4, G, Tag> const &v) noexcept
            : data{{static_cast<float>(v.data[0]),
                    static_cast<float>(v.data[1]),
                    static_cast<float>(v.data[2]),
                    static_cast<float>(v.data[3])}}
        {
        }

        static constexpr auto fill(float c) noexcept
        {
            return abstract_vector<4, float, Tag>{c, c, c, c};
        }

        static constexpr auto zero() noexcept
        {
            return fill(0.0f);
        }
    };

    template <class Tag>
    class abstract_vector<3, float, Tag> {
    public:
        using field_type = float;
        alignas(16) std::array<float, 3> data;
        float padding0 = 0.0f;

        constexpr abstract_vector(float x, float y, float z) noexcept
            : data({{x, y, z}})
        {
        }

        template <class G>
        explicit constexpr abstract_vector(abstract_vector<3, G, Tag> const &v) noexcept
            : data{{static_cast<float>(v.data[0]),
                    static_cast<float>(v.data[1]),
                    static_cast<float>(v.data[2])}}
        {
        }

        static constexpr auto fill(float c) noexcept
        {
            return abstract_vector<3, float, Tag>{c, c, c};
        }

        // EXCL_START cannot test obligatory constexpr function
        static constexpr auto zero() noexcept
        {
            return fill(0.0f);
        }
        // EXCL_STOP
    };

    namespace simd {
        template <class Tag>
        inline auto pack(abstract_vector<4, float, Tag> v) noexcept
        {
            return _mm_load_ps(v.data.data());
        }

        template <class Tag>
        inline auto pack(abstract_vector<3, float, Tag> v) noexcept
        {
            return _mm_load_ps(v.data.data());
        }

        inline auto pack(float c) noexcept
        {
            return _mm_set_ps1(c);
        }

        template <class Tag>
        inline auto unpack4(__m128 v) noexcept
        {
            auto rv = abstract_vector<4, float, Tag>::zero();
            _mm_store_ps(rv.data.data(), v);
            return rv;
        }

        template <class Tag>
        inline auto unpack3(__m128 v) noexcept
        {
            auto rv = abstract_vector<3, float, Tag>::zero();
            _mm_store_ps(rv.data.data(), v);
            return rv;
        }

        inline float unpack1(__m128 v) noexcept
        {
            alignas(16) std::array<float, 4> rv{{}};
            _mm_store_ps1(rv.data(), v);
            return rv[0];
        }

        inline auto vdot(__m128 a, __m128 b) noexcept
        {
            auto vmul = _mm_mul_ps(a, b);
            auto hadd0 = _mm_hadd_ps(vmul, vmul);
            auto hadd1 = _mm_hadd_ps(hadd0, hadd0);
            return hadd1;
        }
    }

    // Vector operations
    template <class TagA, class TagB>
    inline constexpr auto operator+(abstract_vector<4, float, TagA> a,
                                    abstract_vector<4, float, TagB> b)
    {
        using RvTag = decltype(TagA{} + TagB{});
        return simd::unpack4<RvTag>(_mm_add_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator+(abstract_vector<3, float, TagA> a,
                                    abstract_vector<3, float, TagB> b)
    {
        using RvTag = decltype(TagA{} + TagB{});
        return simd::unpack3<RvTag>(_mm_add_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator-(abstract_vector<4, float, TagA> a,
                                    abstract_vector<4, float, TagB> b)
    {
        using RvTag = decltype(TagA{} - TagB{});
        return simd::unpack4<RvTag>(_mm_sub_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator-(abstract_vector<3, float, TagA> a,
                                    abstract_vector<3, float, TagB> b)
    {
        using RvTag = decltype(TagA{} - TagB{});
        return simd::unpack3<RvTag>(_mm_sub_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator*(abstract_vector<4, float, TagA> a,
                                    abstract_vector<4, float, TagB> b)
    {
        using RvTag = decltype(TagA{} * TagB{});
        return simd::unpack4<RvTag>(_mm_mul_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator*(abstract_vector<3, float, TagA> a,
                                    abstract_vector<3, float, TagB> b)
    {
        using RvTag = decltype(TagA{} * TagB{});
        return simd::unpack3<RvTag>(_mm_mul_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator/(abstract_vector<4, float, TagA> a,
                                    abstract_vector<4, float, TagB> b)
    {
        using RvTag = decltype(TagA{} / TagB{});
        return simd::unpack4<RvTag>(_mm_div_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto operator/(abstract_vector<3, float, TagA> a,
                                    abstract_vector<3, float, TagB> b)
    {
        using RvTag = decltype(TagA{} / TagB{});
        return simd::unpack3<RvTag>(_mm_div_ps(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto dot(abstract_vector<4, float, TagA> a, abstract_vector<4, float, TagB> b)
    {
        return simd::unpack1(simd::vdot(simd::pack(a), simd::pack(b)));
    }

    template <class TagA, class TagB>
    inline constexpr auto dot(abstract_vector<3, float, TagA> a, abstract_vector<3, float, TagB> b)
    {
        return simd::unpack1(simd::vdot(simd::pack(a), simd::pack(b)));
    }

    template <class Tag>
    inline constexpr auto length_squared(abstract_vector<4, float, Tag> v)
    {
        auto v_pack = simd::pack(v);
        return simd::unpack1(simd::vdot(v_pack, v_pack));
    }

    template <class Tag>
    inline constexpr auto length_squared(abstract_vector<3, float, Tag> v)
    {
        auto v_pack = simd::pack(v);
        return simd::unpack1(simd::vdot(v_pack, v_pack));
    }

    template <class Tag>
    inline constexpr auto normalize(abstract_vector<4, float, Tag> v)
    {
        auto v_pack = simd::pack(v);
        auto len = simd::vdot(v_pack, v_pack);
        auto rsqlen = _mm_rsqrt_ps(len);
        return simd::unpack4<Tag>(_mm_mul_ps(v_pack, rsqlen));
    }

    template <class Tag>
    inline constexpr auto normalize(abstract_vector<3, float, Tag> v)
    {
        auto v_pack = simd::pack(v);
        auto len = simd::vdot(v_pack, v_pack);
        auto rsqlen = _mm_rsqrt_ps(len);
        return simd::unpack3<Tag>(_mm_mul_ps(v_pack, rsqlen));
    }

    // Scaling operations
    template <class Tag>
    inline constexpr auto operator*(abstract_vector<4, float, Tag> v, float c)
    {
        return simd::unpack4<Tag>(_mm_mul_ps(simd::pack(v), simd::pack(c)));
    }

    template <class Tag>
    inline constexpr auto operator*(abstract_vector<3, float, Tag> v, float c)
    {
        return simd::unpack3<Tag>(_mm_mul_ps(simd::pack(v), simd::pack(c)));
    }

    template <class Tag>
    inline constexpr auto operator*(float c, abstract_vector<4, float, Tag> v)
    {
        return simd::unpack4<Tag>(_mm_mul_ps(simd::pack(c), simd::pack(v)));
    }

    template <class Tag>
    inline constexpr auto operator*(float c, abstract_vector<3, float, Tag> v)
    {
        return simd::unpack3<Tag>(_mm_mul_ps(simd::pack(c), simd::pack(v)));
    }

    template <class Tag>
    inline constexpr auto operator/(abstract_vector<4, float, Tag> v, float c)
    {
        return simd::unpack4<Tag>(_mm_div_ps(simd::pack(v), simd::pack(c)));
    }

    template <class Tag>
    inline constexpr auto operator/(abstract_vector<3, float, Tag> v, float c)
    {
        return simd::unpack3<Tag>(_mm_div_ps(simd::pack(v), simd::pack(c)));
    }
}