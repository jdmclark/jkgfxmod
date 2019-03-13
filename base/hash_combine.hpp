#pragma once

#include <functional>

namespace jkgm {
    template <class T, class SubHashT = std::hash<T>>
    size_t hash_combine(size_t curr, T const &v)
    {
        SubHashT hasher;
        return curr ^ (hasher(v) + 0x9e3779b9U + (curr << 6U) + (curr >> 2U));
    }
}
