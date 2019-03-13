#include "md5.hpp"
#include "hash_combine.hpp"
#include <algorithm>
#include <cstring>

namespace jkgm {
    namespace {
        char md5_lut_s[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                              5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                              4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                              6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

        uint32_t md5_lut_k[64] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613,
            0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
            0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d,
            0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122,
            0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
            0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
            0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb,
            0xeb86d391};

        inline uint32_t md5_left_rotate(uint32_t x, uint32_t c)
        {
            return (x << c) | (x >> (32 - c));
        }

        inline uint32_t md5_little_to_big_endian(uint32_t v)
        {
            auto rv_span = make_span(&v, 1).as_bytes();
            std::reverse(rv_span.begin(), rv_span.end());
            return v;
        }

        uint32_t string_digit_to_md5_digit(char m)
        {
            if(m >= '0' && m <= '9') {
                return (m - '0');
            }

            if(m >= 'A' && m <= 'F') {
                return (m - 'A') + 10;
            }

            if(m >= 'a' && m <= 'f') {
                return (m - 'a') + 10;
            }

            throw std::runtime_error("md5 string contains illegal characters");
        }

        uint32_t string_part_to_md5_part(std::string_view m)
        {
            uint32_t out = 0;
            for(char ch : m) {
                out <<= 4;
                uint32_t hex_digit = string_digit_to_md5_digit(ch);
                out |= hex_digit;
            }

            return out;
        }

        char md5_digit_to_string_digit(char v)
        {
            if(v >= 10) {
                return 'a' + (v - 10);
            }

            return '0' + v;
        }

        void md5_part_to_string_part(uint32_t v, std::string *outbuf)
        {
            for(int i = 28; i >= 0; i -= 4) {
                outbuf->push_back(
                    md5_digit_to_string_digit((v >> static_cast<uint32_t>(i)) & 0xFU));
            }
        }
    }
}

jkgm::md5::md5(std::string_view m)
{
    if(m.size() != 32) {
        throw std::runtime_error("md5 string size mismatch");
    }

    std::get<0>(values) = string_part_to_md5_part(m.substr(0, 8));
    std::get<1>(values) = string_part_to_md5_part(m.substr(8, 8));
    std::get<2>(values) = string_part_to_md5_part(m.substr(16, 8));
    std::get<3>(values) = string_part_to_md5_part(m.substr(24, 8));
}

jkgm::md5::operator std::string() const
{
    std::string outbuf;
    outbuf.reserve(32);

    md5_part_to_string_part(std::get<0>(values), &outbuf);
    md5_part_to_string_part(std::get<1>(values), &outbuf);
    md5_part_to_string_part(std::get<2>(values), &outbuf);
    md5_part_to_string_part(std::get<3>(values), &outbuf);

    return outbuf;
}

bool jkgm::md5::operator<(md5 const &m) const
{
    return values < m.values;
}

bool jkgm::md5::operator==(md5 const &m) const
{
    return values == m.values;
}

bool jkgm::md5::operator!=(md5 const &m) const
{
    return values != m.values;
}

size_t jkgm::md5::hash() const
{
    size_t value = 0;

    value = hash_combine(value, std::get<0>(values));
    value = hash_combine(value, std::get<1>(values));
    value = hash_combine(value, std::get<2>(values));
    value = hash_combine(value, std::get<3>(values));

    return value;
}

std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> jkgm::md5::as_tuple() const
{
    return values;
}

jkgm::md5_hasher::md5_hasher()
    : chunk_span(make_span(chunk).as_bytes())
{
    clear();
}

void jkgm::md5_hasher::clear()
{
    a0 = 0x67452301UL;
    b0 = 0xefcdab89UL;
    c0 = 0x98badcfeUL;
    d0 = 0x10325476UL;
    total_bytes = 0;
}

void jkgm::md5_hasher::add_chunk()
{
    uint32_t A = a0;
    uint32_t B = b0;
    uint32_t C = c0;
    uint32_t D = d0;

    for(uint32_t i = 0; i < 64; ++i) {
        uint32_t F = 0;
        uint32_t g = 0;

        if(i < 16) {
            F = (B & C) | ((~B) & D);
            g = i;
        }
        else if(i < 32) {
            F = (D & B) | ((~D) & C);
            g = (5U * i + 1U) & 0xFU;
        }
        else if(i < 48) {
            F = (B ^ C ^ D);
            g = (3U * i + 5U) & 0xFU;
        }
        else {
            F = (C ^ (B | (~D)));
            g = (7U * i) & 0xFU;
        }

        uint32_t D_temp = D;
        D = C;
        C = B;
        B = B + md5_left_rotate((A + F + md5_lut_k[i] + chunk[g]), md5_lut_s[i]); // NOLINT
        A = D_temp;
    }

    a0 += A;
    b0 += B;
    c0 += C;
    d0 += D;
}

void jkgm::md5_hasher::add(span<char const> src)
{
    while(!src.empty()) {
        size_t actual_amt_written = std::min(src.size(), chunk_span.size());
        memcpy(chunk_span.data(), src.data(), actual_amt_written);
        chunk_span = chunk_span.subspan(actual_amt_written, span_to_end);
        src = src.subspan(actual_amt_written, span_to_end);
        total_bytes += actual_amt_written;

        if(chunk_span.empty()) {
            chunk_span = make_span(chunk).as_bytes();
            add_chunk();
        }
    }
}

jkgm::md5 jkgm::md5_hasher::finish()
{
    uint64_t original_size = total_bytes * 8;

    // Message padding begins with a 1 bit, and is followed by some number of 0 bits.
    // There is always enough room to append the first padding byte to the message.
    unsigned char fin_byte = 0x80U;
    memcpy(chunk_span.data(), &fin_byte, sizeof(fin_byte));
    chunk_span = chunk_span.subspan(1, span_to_end);

    // Check to see if the length can be appended to this chunk
    if(chunk_span.size() < 8) {
        // It can't. Zero out the rest of this chunk and commit it.
        memset(chunk_span.data(), 0, chunk_span.size());

        add_chunk();
        chunk_span = make_span(chunk).as_bytes();
    }

    // Zero out the chunk up to the last 8 bytes
    memset(chunk_span.data(), 0, chunk_span.size() - 8);
    chunk_span = chunk_span.subspan(chunk_span.size() - 8, span_to_end);

    // Append original size, apparently in little endian
    memcpy(chunk_span.data(), &original_size, sizeof(original_size));
    add_chunk();

    // Convert to big endian.
    a0 = md5_little_to_big_endian(a0);
    b0 = md5_little_to_big_endian(b0);
    c0 = md5_little_to_big_endian(c0);
    d0 = md5_little_to_big_endian(d0);

    return md5(a0, b0, c0, d0);
}
