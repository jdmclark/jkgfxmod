#include "virtual_container_iterator.hpp"
#include "base/log.hpp"
#include "virtual_container.hpp"

void jkgm::virtual_container_iterator::set_index(size_t new_index)
{
    current_index = new_index;
    if(new_index < underlying_container->size()) {
        current_file = &underlying_container->get_file(new_index);
    }
    else {
        current_file = std::nullopt;
    }
}

jkgm::virtual_container_iterator::virtual_container_iterator(virtual_container const &cnt,
                                                             size_t current_index)
    : underlying_container(&cnt)
    , current_index(0)
{
    set_index(current_index);
}

jkgm::virtual_file const &jkgm::virtual_container_iterator::operator*() const
{
    if(!current_file.has_value()) {
        throw std::logic_error("virtual container iterator out of bounds");
    }

    return *current_file.value();
}

jkgm::virtual_file const *jkgm::virtual_container_iterator::operator->() const
{
    if(!current_file.has_value()) {
        throw std::logic_error("virtual container iterator out of bounds");
    }

    return current_file.value();
}

jkgm::virtual_container_iterator &jkgm::virtual_container_iterator::operator++()
{
    set_index(current_index + 1);
    return *this;
}

jkgm::virtual_container_iterator jkgm::virtual_container_iterator::operator++(int)
{
    virtual_container_iterator vci(*this);
    set_index(current_index + 1);
    return vci;
}

bool jkgm::virtual_container_iterator::operator==(virtual_container_iterator const &it) const
{
    size_t uc_sz = underlying_container->size();
    bool indices_oob = (current_index >= uc_sz) && (it.current_index >= uc_sz);
    bool indices_equal = (current_index == it.current_index) || indices_oob;
    return (underlying_container == it.underlying_container) && indices_equal;
}

bool jkgm::virtual_container_iterator::operator!=(virtual_container_iterator const &it) const
{
    return !(*this == it);
}
