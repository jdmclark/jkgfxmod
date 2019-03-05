#pragma once

#include "id.hpp"

namespace jkgm {
    class uid_tag {
    };
    using uid = id<uid_tag, uint32_t>;

    template <class T>
    uid uid_of()
    {
        return uid(static_cast<uint32_t>(T::UID::UID));
    }

    template <class T>
    uid uid_of(T const & /*value*/)
    {
        return uid(static_cast<uint32_t>(T::UID::UID));
    }
}

#define UID(x) enum class UID : uint32_t { UID = x##UL }
