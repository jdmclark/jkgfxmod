#include "string_search.hpp"
#include "char.hpp"

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

bool jkgm::iends_with(std::string_view searched, std::string_view prefix)
{
    auto it = searched.rbegin();
    auto jt = prefix.rbegin();

    while(it != searched.rend() && jt != prefix.rend() && to_lower(*it) == to_lower(*jt)) {
        ++it;
        ++jt;
    }

    return jt == prefix.rend();
}
