#include "virtual_file.hpp"

jkgm::virtual_file::virtual_file(fs::path const &fn)
    : name(fn)
{
    return;
}

jkgm::virtual_file::~virtual_file()
{
    return;
}
