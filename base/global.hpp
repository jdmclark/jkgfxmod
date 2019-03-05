#pragma once

#include <memory>
#include <typeindex>
#include <vector>

namespace jkgm {
    class global;

    namespace detail {
        template <class GlobalT>
        class global_factory;
    }

    class global_constructor_protector_tag {
        template <class GlobalT>
        friend class jkgm::detail::global_factory;

    private:
        global_constructor_protector_tag() = default;
    };

    namespace detail {
        class abstract_global_factory {
        public:
            abstract_global_factory() = default;
            virtual ~abstract_global_factory() = default;

            abstract_global_factory(abstract_global_factory const &) = delete;
            abstract_global_factory(abstract_global_factory &&) = delete;
            abstract_global_factory &operator=(abstract_global_factory const &) = delete;
            abstract_global_factory &operator=(abstract_global_factory &&) = delete;

            virtual std::shared_ptr<global> make_global() const = 0;
        };

        template <class GlobalT>
        class global_factory : public abstract_global_factory {
        public:
            std::shared_ptr<global> make_global() const override
            {
                return std::make_shared<GlobalT>(global_constructor_protector_tag());
            }
        };

        std::shared_ptr<global> get_or_create_global(std::type_index tid,
                                                     abstract_global_factory const &factory);
    }

    class global {
    private:
        std::vector<std::shared_ptr<global>> reserved_global_dependencies;

        std::shared_ptr<global>
            reserve_global_dependency_with_factory(std::type_index tid,
                                                   detail::abstract_global_factory const &factory);

    protected:
        template <class GlobalT>
        std::shared_ptr<GlobalT> reserve_global_dependency()
        {
            auto ptr = reserve_global_dependency_with_factory(typeid(GlobalT),
                                                              detail::global_factory<GlobalT>());
            return std::dynamic_pointer_cast<GlobalT>(ptr);
        }

        explicit global(global_constructor_protector_tag /*tag*/);

    public:
        virtual ~global() = default;

        global() = delete;
        global(global const &) = delete;
        global(global &&) = delete;
        global &operator=(global const &) = delete;
        global &operator=(global &&) = delete;
    };

    template <class GlobalT>
    std::shared_ptr<GlobalT> get_global()
    {
        auto g = get_or_create_global(typeid(GlobalT), detail::global_factory<GlobalT>());
        return std::dynamic_pointer_cast<GlobalT>(g);
    }
}
