#pragma once

#include "narrow.hpp"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace jkgm {
    template <class ViewT>
    class cstring_view_iterator {
    private:
        ViewT const *underlying_view;
        size_t index;

        using element_type = typename ViewT::element_type;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_cv_t<element_type>;
        using pointer = element_type const *;
        using reference = element_type const &;
        using iterator_category = std::random_access_iterator_tag;

        constexpr cstring_view_iterator(ViewT const &underlying_view, size_t index)
            : underlying_view(&underlying_view)
            , index(index)
        {
        }

        constexpr reference operator*() const
        {
            return underlying_view->base[index];
        }

        constexpr pointer operator->() const
        {
            return &underlying_view->base[index];
        }

        constexpr cstring_view_iterator &operator++()
        {
            ++index;
            return *this;
        }

        constexpr cstring_view_iterator const operator++(int)
        {
            auto rv = *this;
            ++index;
            return rv;
        }

        constexpr cstring_view_iterator &operator--()
        {
            --index;
            return *this;
        }

        constexpr cstring_view_iterator const operator--(int)
        {
            auto rv = *this;
            --index;
            return rv;
        }

        constexpr cstring_view_iterator operator+(ptrdiff_t offset) const
        {
            return {*underlying_view, index + offset};
        }

        constexpr cstring_view_iterator &operator+=(ptrdiff_t offset)
        {
            index += offset;
            return *this;
        }

        constexpr cstring_view_iterator operator-(ptrdiff_t offset) const
        {
            return {*underlying_view, index - offset};
        }

        constexpr cstring_view_iterator &operator-=(ptrdiff_t offset)
        {
            index -= offset;
            return *this;
        }

        constexpr ptrdiff_t operator-(cstring_view_iterator const &other) const
        {
            assert(underlying_view == other.underlying_view &&
                   "cstring_view_iterator computed difference from multiple views");
            return narrow<ptrdiff_t>(index) - narrow<ptrdiff_t>(other.index);
        }

        constexpr bool operator==(cstring_view_iterator const &other) const
        {
            assert(underlying_view == other.underlying_view &&
                   "cstring_view_iterator compared multiple views");
            return index == other.index;
        }

        constexpr bool operator!=(cstring_view_iterator const &other) const
        {
            return !(*this == other);
        }

        constexpr bool operator<(cstring_view_iterator const &other) const
        {
            assert(underlying_view == other.underlying_view &&
                   "cstring_view_iterator compared multiple views");
            return index < other.index;
        }

        constexpr bool operator<=(cstring_view_iterator const &other) const
        {
            return !(other < *this);
        }

        constexpr bool operator>(cstring_view_iterator const &other) const
        {
            return other < *this;
        }

        constexpr bool operator>=(cstring_view_iterator const &other) const
        {
            return !(*this < other);
        }
    };

    template <class EmT>
    class basic_cstring_view {
        friend cstring_view_iterator<basic_cstring_view>;

    private:
        EmT const *base = nullptr;
        size_t count = 0;

    public:
        using element_type = EmT;
        using const_iterator = cstring_view_iterator<basic_cstring_view>;
        using iterator = const_iterator;

        constexpr basic_cstring_view(EmT const *base, size_t count) noexcept
            : base(base)
            , count(count)
        {
        }

        constexpr basic_cstring_view(std::basic_string<EmT> const &value) noexcept
            : base(value.data())
            , count(value.size())
        {
        }

        template <size_t N>
        constexpr basic_cstring_view(EmT const (&arr)[N]) noexcept
            : base(static_cast<EmT const *>(arr))
            , count(N - 1)
        {
        }

        constexpr basic_cstring_view(EmT const *cstr) noexcept
            : base(cstr)
            , count(std::strlen(cstr))
        {
        }

        iterator begin()
        {
            return {*this, 0};
        }

        iterator end()
        {
            return {*this, count};
        }

        const_iterator begin() const
        {
            return {*this, 0};
        }

        const_iterator end() const
        {
            return {*this, count};
        }

        const_iterator cbegin() const
        {
            return {*this, 0};
        }

        const_iterator cend() const
        {
            return {*this, count};
        }

        std::basic_string_view<EmT> substr(size_t start, size_t num) const
        {
            assert(start <= count && "basic_cstring_view::substr starts past end of view");
            assert((start + num) <= count &&
                   "basic_cstring_view::substr exceeds dimensions of view");
            return {base + start, num};
        }

        size_t size() const
        {
            return count;
        }

        bool empty() const
        {
            return count == 0;
        }

        EmT const *data() const
        {
            return base;
        }

        EmT const *c_str() const
        {
            return base;
        }

        operator std::basic_string_view<EmT>() const noexcept
        {
            return {base, count};
        }

        bool operator==(basic_cstring_view const &other) const
        {
            return std::equal(begin(), end(), other.begin(), other.end());
        }

        bool operator!=(basic_cstring_view const &other) const
        {
            return !std::equal(begin(), end(), other.begin(), other.end());
        }
    };

    using cstring_view = basic_cstring_view<char>;

    namespace literals {
        constexpr cstring_view operator"" _csv(char const *str, size_t len) noexcept
        {
            return {str, len};
        }
    }
}
