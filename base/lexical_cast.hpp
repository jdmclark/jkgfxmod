#pragma once

#include "string_search.hpp"
#include <string>
#include <string_view>

namespace jkgm {
    template <class T, class = void>
    struct lexical_cast_converter {
        std::string convert(std::string_view sv) const
        {
            return std::string(sv.data(), sv.size());
        }
    };

    template <>
    struct lexical_cast_converter<bool> {
        bool convert(std::string_view sv) const
        {
            return iequal(sv, std::string_view("true"));
        }
    };

    template <>
    struct lexical_cast_converter<float> {
        float convert(std::string_view sv) const
        {
            return std::stof(std::string(sv.data(), sv.size()));
        }
    };

    template <>
    struct lexical_cast_converter<double> {
        double convert(std::string_view sv) const
        {
            return std::stod(std::string(sv.data(), sv.size()));
        }
    };

    template <>
    struct lexical_cast_converter<long double> {
        long double convert(std::string_view sv) const
        {
            return std::stold(std::string(sv.data(), sv.size()));
        }
    };

    template <>
    struct lexical_cast_converter<int> {
        int convert(std::string_view sv) const
        {
            return std::stoi(std::string(sv.data(), sv.size()), nullptr, 0);
        }
    };

    template <>
    struct lexical_cast_converter<unsigned int> {
        int convert(std::string_view sv) const
        {
            return std::stoul(std::string(sv.data(), sv.size()), nullptr, 0);
        }
    };

    template <>
    struct lexical_cast_converter<int64_t> {
        int64_t convert(std::string_view sv) const
        {
            static_assert(sizeof(long long int) == sizeof(int64_t) || // NOLINT: int required
                              sizeof(long int) == sizeof(int64_t), // NOLINT: int required
                          "lexical_cast_converter<int64_t> not implemented");

            if constexpr(sizeof(long long int) == sizeof(int64_t)) { // NOLINT: int required
                return std::stoll(std::string(sv.data(), sv.size()), nullptr, 0);
            }

            if constexpr(sizeof(long int) == sizeof(int64_t)) { // NOLINT: int required
                return std::stol(std::string(sv.data(), sv.size()), nullptr, 0);
            }
        }
    };

    template <>
    struct lexical_cast_converter<uint64_t> {
        uint64_t convert(std::string_view sv) const
        {
            static_assert(sizeof(unsigned long long int) == sizeof(uint64_t) || // NOLINT
                              sizeof(unsigned long int) == sizeof(uint64_t), // NOLINT
                          "lexical_cast_converter<uint64_t> not implemented");

            if constexpr(sizeof(unsigned long long int) == sizeof(uint64_t)) { // NOLINT
                return std::stoull(std::string(sv.data(), sv.size()), nullptr, 0);
            }

            if constexpr(sizeof(unsigned long int) == sizeof(uint64_t)) { // NOLINT
                return std::stoul(std::string(sv.data(), sv.size()), nullptr, 0);
            }
        }
    };

    template <class T>
    using lexical_cast_converter_type_t = lexical_cast_converter<std::decay_t<T>>;

    template <class T, class ConvT = lexical_cast_converter_type_t<T>>
    T lexical_cast(std::string_view str)
    {
        return ConvT().convert(str);
    }
}
