#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace jkgm {
    template <class T, class = void>
    struct formatter {
    };

    class format_buffer {
    protected:
        format_buffer() = default;

    public:
        virtual ~format_buffer() = default;

        format_buffer(format_buffer const &) = delete;
        format_buffer(format_buffer &&) = delete;
        format_buffer &operator=(format_buffer const &) = delete;
        format_buffer &operator=(format_buffer &&) = delete;

        virtual void append(std::string_view value) = 0;
    };

    class format_script {
    protected:
        format_script() = default;

    public:
        virtual ~format_script() = default;

        format_script(format_script const &) = default;
        format_script(format_script &&) = default;
        format_script &operator=(format_script const &) = default;
        format_script &operator=(format_script &&) = default;

        virtual void fill_buffer(format_buffer *buffer) const = 0;
        virtual size_t estimate_buffer() const = 0;
    };

    namespace detail {
        template <class T>
        formatter<T> declfmtval(formatter<T> const &v, int /*ignored*/);

        template <class T>
        formatter<std::decay_t<T>> declfmtval(T const &v, ...);
    }

    template <class T>
    using formatter_type_t = decltype(detail::declfmtval(std::declval<T>(), 0));

    namespace detail {
        class formatter_fill_op {
        private:
            jkgm::format_buffer *buffer;

        public:
            explicit formatter_fill_op(jkgm::format_buffer *buffer);

            template <class T>
            void operator()(T const &fmt)
            {
                fmt.fill_buffer(buffer);
            }
        };

        class formatter_sequence_fill_op {
        private:
            format_buffer *buffer;

        public:
            explicit formatter_sequence_fill_op(format_buffer *buffer);

            template <class... T>
            void operator()(T const &... fmt)
            {
                formatter_fill_op fill_op(buffer);
                (fill_op(fmt), ...);
            }
        };

        class formatter_est_op {
        public:
            size_t out_estimate = 0;

            template <class T>
            void operator()(T const &fmt)
            {
                out_estimate += fmt.estimate_buffer();
            }
        };

        class formatter_sequence_est_op {
        public:
            template <class... T>
            size_t operator()(T const &... fmt)
            {
                formatter_est_op est_op;
                (est_op(fmt), ...);
                return est_op.out_estimate;
            }
        };
    }

    template <class... T>
    class formatter_sequence : public format_script {
    private:
        std::tuple<T...> formatters;

    public:
        explicit formatter_sequence(T... formatters)
            : formatters(formatters...)
        {
        }

        void fill_buffer(format_buffer *buffer) const override
        {
            detail::formatter_sequence_fill_op fill_op(buffer);
            std::apply(fill_op, formatters);
        }

        size_t estimate_buffer() const override
        {
            detail::formatter_sequence_est_op est_op;
            return std::apply(est_op, formatters);
        }
    };

    template <class T>
    struct formatter<T, std::enable_if_t<std::is_convertible_v<T const &, format_script const &>>> {
    private:
        format_script const &value;

    public:
        formatter(format_script const &value)
            : value(value)
        {
        }

        void fill_buffer(format_buffer *buffer) const
        {
            value.fill_buffer(buffer);
        }

        size_t estimate_buffer() const
        {
            return value.estimate_buffer();
        }
    };

    template <class... T>
    auto format(T &&... v)
    {
        return formatter_sequence<formatter_type_t<T>...>(
            std::forward<T>(v)...);
    }

    namespace detail {
        class string_format_buffer : public format_buffer {
        private:
            std::string *buffer;

        public:
            explicit string_format_buffer(std::string *buffer);
            void append(std::string_view value) override;
        };
    }

    template <class FmtSeqT>
    std::string str(FmtSeqT const &fmt_seq)
    {
        std::string buffer;
        buffer.reserve(fmt_seq.estimate_buffer());
        detail::string_format_buffer sfb(&buffer);

        fmt_seq.fill_buffer(&sfb);
        return buffer;
    }
}
