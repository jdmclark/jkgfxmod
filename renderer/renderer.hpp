#pragma once

#include "base/span.hpp"
#include <Windows.h>
#include <memory>

namespace jkgm {
    class renderer {
    public:
        virtual ~renderer() = default;

        virtual void initialize() = 0;
        virtual void set_menu_palette(UINT start, span<RGBQUAD const> entries) = 0;
        virtual void set_menu_source(char const *indexed_bitmap) = 0;
        virtual void present_menu() = 0;
    };

    std::unique_ptr<renderer> create_renderer(HINSTANCE dll_instance);
}