#pragma once

#include "lerp.hpp"
#include <utility>

namespace jkgm {
    template <class F, class Tag>
    class abstract_scalar {
    private:
        F data;

    public:
        using field_type = F;

        abstract_scalar() = delete;
        ~abstract_scalar() = default;

        abstract_scalar(abstract_scalar const &) = default;
        abstract_scalar(abstract_scalar &&) noexcept = default;
        abstract_scalar &operator=(abstract_scalar const &) = default;
        abstract_scalar &operator=(abstract_scalar &&) noexcept = default;

        explicit abstract_scalar(F value) noexcept
            : data(std::forward<F>(value))
        {
        }

        template <class G>
        explicit constexpr abstract_scalar(abstract_scalar<G, Tag> const &v) noexcept
            : data(static_cast<F>(v.data))
        {
        }

        abstract_scalar &operator=(F &&v) noexcept
        {
            data = std::forward<F>(v);
            return *this;
        }

        constexpr F &get() noexcept
        {
            return data;
        }

        constexpr F get() const noexcept
        {
            return data;
        }
    };

    template <class F, class Tag>
    constexpr F &get(abstract_scalar<F, Tag> &v) noexcept
    {
        return v.get();
    }

    template <class F, class Tag>
    constexpr F get(abstract_scalar<F, Tag> const &v) noexcept
    {
        return v.get();
    }

    template <class NewTag, class F, class Tag>
    constexpr auto force_retag(abstract_scalar<F, Tag> const &v) noexcept
    {
        return abstract_scalar<F, NewTag>(get(v));
    }

    template <class F, class Tag>
    constexpr bool operator==(abstract_scalar<F, Tag> const &a, abstract_scalar<F, Tag> const &b)
    {
        return get(a) == get(b);
    }

    template <class F, class Tag>
    constexpr bool operator!=(abstract_scalar<F, Tag> const &a, abstract_scalar<F, Tag> const &b)
    {
        return get(a) != get(b);
    }

    template <class F, class TagA, class TagB>
    constexpr auto operator+(abstract_scalar<F, TagA> const &a, abstract_scalar<F, TagB> const &b)
    {
        return abstract_scalar<F, decltype(TagA() + TagB())>(get(a) + get(b));
    }

    template <class F, class TagA, class TagB>
    constexpr auto operator-(abstract_scalar<F, TagA> const &a, abstract_scalar<F, TagB> const &b)
    {
        return abstract_scalar<F, decltype(TagA() - TagB())>(get(a) - get(b));
    }

    template <class F, class TagA, class TagB>
    constexpr auto operator*(abstract_scalar<F, TagA> const &a, abstract_scalar<F, TagB> const &b)
    {
        return abstract_scalar<F, decltype(TagA() * TagB())>(get(a) * get(b));
    }

    template <class F, class Tag>
    constexpr auto operator*(abstract_scalar<F, Tag> const &a, F const &b)
    {
        return abstract_scalar<F, Tag>(get(a) * b);
    }

    template <class F, class Tag>
    constexpr auto operator*(F const &a, abstract_scalar<F, Tag> const &b)
    {
        return abstract_scalar<F, Tag>(a * get(b));
    }

    template <class F, class TagA, class TagB>
    constexpr auto operator/(abstract_scalar<F, TagA> const &a, abstract_scalar<F, TagB> const &b)
    {
        return abstract_scalar<F, decltype(TagA() / TagB())>(get(a) / get(b));
    }

    template <class F, class Tag>
    constexpr auto operator/(abstract_scalar<F, Tag> const &a, F const &b)
    {
        return abstract_scalar<F, Tag>(get(a) / b);
    }

    template <class F, class Tag>
    constexpr auto operator/(F const &a, abstract_scalar<F, Tag> const &b)
    {
        return abstract_scalar<F, Tag>(a / get(b));
    }

    template <class F, class Tag>
    constexpr auto operator-(abstract_scalar<F, Tag> const &a)
    {
        return abstract_scalar<F, Tag>(-get(a));
    }
}
