#pragma once

#include "base/id.hpp"
#include <cstddef>

namespace jkgm {
    MAKE_ID_TYPE(srgb_texture, size_t);
    MAKE_ID_TYPE(material_instance, size_t);

    class renderer;
}