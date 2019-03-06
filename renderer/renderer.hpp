#pragma once

#include "base/span.hpp"
#include <Windows.h>
#include <d3d.h>
#include <ddraw.h>
#include <memory>

namespace jkgm {
    class renderer {
    public:
        virtual ~renderer() = default;

        virtual void initialize() = 0;

        virtual HRESULT enumerate_devices(LPDDENUMCALLBACKA cb, LPVOID lpContext) = 0;

        virtual void set_menu_palette(UINT start, span<RGBQUAD const> entries) = 0;
        virtual void set_menu_source(char const *indexed_bitmap) = 0;
        virtual void present_menu() = 0;

        virtual void present_game() = 0;

        virtual IDirectDraw *get_directdraw() = 0;
        virtual IDirectDraw2 *get_directdraw2() = 0;
        virtual IDirect3D *get_direct3d() = 0;
        virtual IDirect3DDevice *get_direct3ddevice() = 0;
        virtual IDirect3DViewport *get_direct3dviewport() = 0;
        virtual IDirect3DTexture *get_direct3dtexture() = 0;
        virtual IDirect3DExecuteBuffer *get_direct3dexecutebuffer(size_t bufsz) = 0;

        virtual IDirectDrawSurface *get_directdraw_primary_surface() = 0;
        virtual IDirectDrawSurface *get_directdraw_backbuffer_surface() = 0;
        virtual IDirectDrawSurface *get_directdraw_phony_surface(DDSURFACEDESC desc) = 0;

        virtual IDirectDrawPalette *get_directdraw_palette(span<PALETTEENTRY const> entries) = 0;
    };

    std::unique_ptr<renderer> create_renderer(HINSTANCE dll_instance);
}