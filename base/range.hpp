#pragma once

#include <tuple>

namespace jkgm {
    using std::get;
    using std::tuple_size;

    template <class It>
    class range {
    private:
        It first, last;

    public:
        range(It first, It last)
            : first(std::move(first))
            , last(std::move(last))
        {
        }

        It const &begin() const
        {
            return first;
        }

        It const &end() const
        {
            return last;
        }

        inline bool empty() const
        {
            return first == last;
        }
    };

    template <class It>
    range<It> make_range(It const &first, It const &last)
    {
        return range<It>(first, last);
    }

    template <class TupleT>
    auto make_range(TupleT const &tuple) -> decltype(make_range(get<0>(tuple), get<1>(tuple)))
    {
        static_assert(tuple_size<TupleT>::value == 2, "tuple must be a pair");
        return make_range(get<0>(tuple), get<1>(tuple));
    }

    template <class RangeT>
    auto make_range(RangeT &rng) -> decltype(make_range(rng.begin(), rng.end()))
    {
        return make_range(rng.begin(), rng.end());
    }

    template <class ContainerT>
    auto make_reverse_range(ContainerT &rng) -> decltype(make_range(rng.rbegin(), rng.rend()))
    {
        return make_range(rng.rbegin(), rng.rend());
    }
}
