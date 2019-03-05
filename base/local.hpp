#pragma once

#include <memory>
#include <typeindex>
#include <vector>

namespace jkgm {
    class local;

    namespace detail {
        template <class LocalT>
        class local_factory;
    }

    class local_constructor_protector_tag {
        template <class LocalT>
        friend class jkgm::detail::local_factory;

    private:
        local_constructor_protector_tag() = default;
    };

    namespace detail {
        class abstract_local_factory {
        public:
            abstract_local_factory() = default;
            virtual ~abstract_local_factory() = default;

            abstract_local_factory(abstract_local_factory const &) = delete;
            abstract_local_factory(abstract_local_factory &&) = delete;
            abstract_local_factory &operator=(abstract_local_factory const &) = delete;
            abstract_local_factory &operator=(abstract_local_factory &&) = delete;

            virtual std::shared_ptr<local> make_local() const = 0;
        };

        template <class LocalT>
        class local_factory : public abstract_local_factory {
        public:
            std::shared_ptr<local> make_local() const override
            {
                return std::make_shared<LocalT>(local_constructor_protector_tag());
            }
        };

        std::shared_ptr<local> get_or_create_local(std::type_index tid,
                                                   abstract_local_factory const &factory);
    }

    class local {
    protected:
        explicit local(local_constructor_protector_tag /*tag*/);

    public:
        virtual ~local() = default;

        local() = delete;
        local(local const &) = delete;
        local(local &&) = delete;
        local &operator=(local const &) = delete;
        local &operator=(local &&) = delete;
    };

    template <class LocalT>
    std::shared_ptr<LocalT> get_local()
    {
        auto g = get_or_create_local(typeid(LocalT), detail::local_factory<LocalT>());
        return std::dynamic_pointer_cast<LocalT>(g);
    }
}
