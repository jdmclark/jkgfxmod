#pragma once

#include "format_base.hpp"
#include <algorithm>
#include <cstdint>

namespace jkgm {
    template <class T>
    struct formatter<T, typename std::enable_if_t<std::is_integral<T>::value>> {
    private:
        enum class int_radix { bin, oct, dec, hex };

        T const &value;
        int_radix radix = int_radix::dec;
        size_t width = 0;

        void append_body(std::string *out, bool *negative, int radix) const
        {
            T temp_value = value;
            *negative = false;

            if constexpr(std::is_signed_v<T>) {
                if(temp_value < 0) {
                    *negative = true;
                    temp_value = -temp_value;
                }
            }

            if(temp_value == 0) {
                out->push_back('0');
            }

            while(temp_value > 0) {
                int part = temp_value % radix;
                if(part < 10) {
                    out->push_back('0' + part);
                }
                else {
                    out->push_back('a' + (part - 10));
                }

                temp_value = temp_value / radix;
            }
        }

        std::string get_string_bin() const
        {
            std::string rv;
            rv.reserve(sizeof(T) * 4 + 3);

            bool negative = false;
            append_body(&rv, &negative, 2);

            rv.push_back('b');
            rv.push_back('0');

            if(negative) {
                rv.push_back('-');
            }

            std::reverse(rv.begin(), rv.end());
            return rv;
        }

        std::string get_string_oct() const
        {
            std::string rv;
            rv.reserve(sizeof(T) * 4 + 3);

            bool negative = false;
            append_body(&rv, &negative, 8);

            if(value != 0) {
                rv.push_back('0');
            }

            if(negative) {
                rv.push_back('-');
            }

            std::reverse(rv.begin(), rv.end());
            return rv;
        }

        std::string get_string_dec() const
        {
            std::string rv;
            rv.reserve(sizeof(T) * 4 + 3);

            bool negative = false;
            append_body(&rv, &negative, 10);

            if(negative) {
                rv.push_back('-');
            }

            std::reverse(rv.begin(), rv.end());
            return rv;
        }

        std::string get_string_hex() const
        {
            std::string rv;
            rv.reserve(sizeof(T) * 4 + 3);

            bool negative = false;
            append_body(&rv, &negative, 16);

            rv.push_back('x');
            rv.push_back('0');

            if(negative) {
                rv.push_back('-');
            }

            std::reverse(rv.begin(), rv.end());
            return rv;
        }

        std::string get_string() const
        {
            switch(radix) {
            case int_radix::bin:
                return get_string_bin();

            case int_radix::oct:
                return get_string_oct();

            case int_radix::dec:
            default:
                return get_string_dec();

            case int_radix::hex:
                return get_string_hex();
            }
        }

    public:
        formatter(T const &value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            auto tmp_s = get_string();
            if(width > tmp_s.size()) {
                for(size_t i = width; i > tmp_s.size(); --i) {
                    buffer->append(" ");
                }
            }

            buffer->append(tmp_s);
        }

        size_t estimate_buffer() const
        {
            // Pessimistic estimate
            return std::max(sizeof(T) * 4 + 3, width);
        }

        void make_bin()
        {
            radix = int_radix::bin;
        }

        void make_oct()
        {
            radix = int_radix::oct;
        }

        void make_dec()
        {
            radix = int_radix::dec;
        }

        void make_hex()
        {
            radix = int_radix::hex;
        }

        void set_width(size_t w)
        {
            width = w;
        }
    };

    template <class T>
    formatter<T> bin(formatter<T> fmt)
    {
        fmt.make_bin();
        return fmt;
    }

    template <class T, class FmtT = formatter_type_t<T>>
    FmtT bin(T const &v)
    {
        FmtT fmt(v);
        fmt.make_bin();
        return fmt;
    }

    template <class T>
    formatter<T> oct(formatter<T> fmt)
    {
        fmt.make_oct();
        return fmt;
    }

    template <class T, class FmtT = formatter_type_t<T>>
    FmtT oct(T const &v)
    {
        FmtT fmt(v);
        fmt.make_oct();
        return fmt;
    }

    template <class T>
    formatter<T> dec(formatter<T> fmt)
    {
        fmt.make_dec();
        return fmt;
    }

    template <class T, class FmtT = formatter_type_t<T>>
    FmtT dec(T const &v)
    {
        FmtT fmt(v);
        fmt.make_dec();
        return fmt;
    }

    template <class T>
    formatter<T> hex(formatter<T> fmt)
    {
        fmt.make_hex();
        return fmt;
    }

    template <class T, class FmtT = formatter_type_t<T>>
    FmtT hex(T const &v)
    {
        FmtT fmt(v);
        fmt.make_hex();
        return fmt;
    }

    template <class T>
    formatter<T> width(size_t w, formatter<T> fmt)
    {
        fmt.set_width(w);
        return fmt;
    }

    template <class T, class FmtT = formatter_type_t<T>>
    FmtT width(size_t w, T const &v)
    {
        FmtT fmt(v);
        fmt.set_width(w);
        return fmt;
    }
}
