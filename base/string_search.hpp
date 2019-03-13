#pragma once

#include "char.hpp"
#include <string_view>

namespace jkgm {
    bool begins_with(std::string_view searched, std::string_view prefix);

    template <class LeftRngT, class RightRngT>
    bool iequal(LeftRngT const &left, RightRngT const &right)
    {
        auto it = left.begin();
        auto jt = right.begin();

        while(it != left.end() && jt != right.end() && to_lower(*it) == to_lower(*jt)) {
            ++it;
            ++jt;
        }

        return it == left.end() && jt == right.end();
    }
}
