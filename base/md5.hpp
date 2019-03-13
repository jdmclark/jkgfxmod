#pragma once

#include "span.hpp"
#include <cstdint>
#include <numeric>
#include <string_view>
#include <tuple>

namespace jkgm {
    class md5 {
    private:
        std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> values;

    public:
        constexpr md5(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
            : values(a, b, c, d)
        {
        }

        explicit md5(std::string_view m);
        explicit operator std::string() const;

        bool operator<(md5 const &m) const;
        bool operator==(md5 const &m) const;
        bool operator!=(md5 const &m) const;

        size_t hash() const;

        std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> as_tuple() const;
    };

    class md5_hasher {
    private:
        uint32_t a0 = 0;
        uint32_t b0 = 0;
        uint32_t c0 = 0;
        uint32_t d0 = 0;

        uint32_t chunk[16] = {0};
        span<char> chunk_span;
        uint64_t total_bytes = 0;

        void add_chunk();

    public:
        md5_hasher();

        void clear();
        void add(span<char const> src);
        md5 finish();
    };
}

namespace std {
    template <>
    struct hash<jkgm::md5> {
        size_t operator()(jkgm::md5 const &m) const
        {
            return m.hash();
        }
    };
}
