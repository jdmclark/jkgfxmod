#pragma once

#include <optional>
#include <utility>

namespace jkgm {
    template <class TraitsT>
    class unique_handle {
    private:
        std::optional<typename TraitsT::value_type> id;

    public:
        unique_handle(unique_handle const &) = delete;
        unique_handle &operator=(unique_handle const &) = delete;

        template <class... ArgT>
        explicit unique_handle(ArgT &&... args)
            : id(TraitsT::create(std::forward<ArgT>(args)...))
        {
        }

        unique_handle(unique_handle &&obj) noexcept
            : id(std::move(obj.id))
        {
            obj.id.reset();
        }

        ~unique_handle()
        {
            if(id.has_value()) {
                TraitsT::destroy(*id);
                id.reset();
            }
        }

        unique_handle &operator=(unique_handle &&obj) noexcept
        {
            if(this != &obj && id.has_value()) {
                TraitsT::destroy(*id);
                id.reset();
            }

            std::swap(id, obj.id);
            return *this;
        }

        inline typename TraitsT::value_type value() const
        {
            return id.value();
        }

        inline typename TraitsT::value_type operator*() const
        {
            return *id;
        }

        typename TraitsT::value_type release()
        {
            typename TraitsT::value_type rv(std::move(id.value()));
            id.reset();
            return rv;
        }
    };

    template <class TraitsT>
    class unique_handle_view {
    private:
        typename TraitsT::value_type id;

    public:
        explicit constexpr unique_handle_view(typename TraitsT::value_type id)
            : id(id)
        {
        }

        unique_handle_view(unique_handle<TraitsT> const &uh)
            : id(uh.value())
        {
        }

        inline typename TraitsT::value_type value() const
        {
            return id;
        }

        inline typename TraitsT::value_type operator*() const
        {
            return id;
        }
    };
}
