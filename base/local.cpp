#include "local.hpp"
#include <unordered_map>

namespace {
    using local_map = std::unordered_map<std::type_index, std::shared_ptr<jkgm::local>>;
    thread_local std::unique_ptr<local_map> local_registry;
}

jkgm::local::local(local_constructor_protector_tag /*unused*/) {}

std::shared_ptr<jkgm::local>
    jkgm::detail::get_or_create_local(std::type_index tid,
                                     detail::abstract_local_factory const &factory)
{
    if(!local_registry) {
        local_registry = std::make_unique<local_map>();
    }

    auto it = local_registry->find(tid);
    if(it == local_registry->end()) {
        it = local_registry->emplace(tid, factory.make_local()).first;
    }

    return it->second;
}
