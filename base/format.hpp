#pragma once

#include "format_base.hpp"
#include "format_numeric.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

namespace jkgm {
    template <>
    struct formatter<bool> {
    private:
        bool value;

    public:
        formatter(bool value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(value ? "true" : "false");
        }

        size_t estimate_buffer() const
        {
            return value ? 4U : 5U;
        }
    };

    template <>
    struct formatter<char> {
    private:
        char value;

    public:
        formatter(char value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(std::string_view(&value, 1));
        }

        size_t estimate_buffer() const
        {
            return 1;
        }
    };

    template <>
    struct formatter<char *> {
    private:
        std::string_view value;

    public:
        formatter(char const *value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(value);
        }

        size_t estimate_buffer() const
        {
            return value.size();
        }
    };

    template <>
    struct formatter<char const *> {
    private:
        std::string_view value;

    public:
        formatter(char const *value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(value);
        }

        size_t estimate_buffer() const
        {
            return value.size();
        }
    };

    template <>
    struct formatter<std::string> {
    private:
        std::string_view value;

    public:
        formatter(std::string const &value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(value);
        }

        size_t estimate_buffer() const
        {
            return value.size();
        }
    };

    template <>
    struct formatter<std::string_view> {
    private:
        std::string_view value;

    public:
        formatter(std::string_view value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            buffer->append(value);
        }

        size_t estimate_buffer() const
        {
            return value.size();
        }
    };
}
