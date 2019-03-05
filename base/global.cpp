#include "global.hpp"
#include <mutex>
#include <unordered_map>

namespace {
    using global_map = std::unordered_map<std::type_index, std::shared_ptr<jkgm::global>>;
    using global_registry = std::tuple<global_map, std::mutex>;

    global_registry *get_global_registry()
    {
        static global_registry reg;
        return &reg;
    }
}

jkgm::global::global(global_constructor_protector_tag /* unused */) {}

std::shared_ptr<jkgm::global> jkgm::global::reserve_global_dependency_with_factory(
    std::type_index tid,
    detail::abstract_global_factory const &factory)
{
    auto &gr = std::get<0>(*get_global_registry());

    auto it = gr.find(tid);
    if(it == gr.end()) {
        it = gr.emplace(tid, factory.make_global()).first;
    }

    reserved_global_dependencies.push_back(it->second);

    return it->second;
}

std::shared_ptr<jkgm::global>
    jkgm::detail::get_or_create_global(std::type_index tid, abstract_global_factory const &factory)
{
    auto *gr_pair = get_global_registry();
    auto &gr = std::get<0>(*gr_pair);
    auto &grmut = std::get<1>(*gr_pair);

    std::lock_guard<std::mutex> lk(grmut);

    auto it = gr.find(tid);
    if(it == gr.end()) {
        it = gr.emplace(tid, factory.make_global()).first;
    }

    return it->second;
}
