#include "string_search.hpp"

bool jkgm::begins_with(std::string_view searched, std::string_view prefix)
{
    auto it = searched.begin();
    auto jt = prefix.begin();

    while(it != searched.end() && jt != prefix.end() && *it == *jt) {
        ++it;
        ++jt;
    }

    return jt == prefix.end();
}
