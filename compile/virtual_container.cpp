#include "virtual_container.hpp"
#include "base/log.hpp"

jkgm::virtual_container::virtual_container(fs::path const &container_filename)
    : container_filename(container_filename)
{
    return;
}

jkgm::virtual_container::~virtual_container()
{
    return;
}

jkgm::virtual_container_iterator jkgm::virtual_container::begin() const
{
    return virtual_container_iterator(*this, 0);
}

jkgm::virtual_container_iterator jkgm::virtual_container::end() const
{
    return virtual_container_iterator(*this, size());
}
