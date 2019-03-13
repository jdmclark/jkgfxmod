#pragma once

#include "base/filesystem.hpp"
#include "base/md5.hpp"
#include "base/span.hpp"
#include "common/config.hpp"
#include "common/material.hpp"
#include "math/point.hpp"
#include "math/size.hpp"
#include <Windows.h>
#include <d3d.h>
#include <ddraw.h>
#include <memory>

namespace jkgm {
    enum class renderer_mode { menu, ingame };

    class renderer {
    public:
        virtual ~renderer() = default;

        virtual void set_renderer_mode(renderer_mode mode) = 0;
        virtual size<2, int> get_configured_screen_resolution() = 0;
        virtual point<2, int> get_cursor_pos(point<2, int> real_pos) = 0;

        virtual void initialize(HWND parentWnd) = 0;

        virtual HRESULT enumerate_devices(LPDDENUMCALLBACKA cb, LPVOID lpContext) = 0;

        virtual void set_menu_palette(UINT start, span<RGBQUAD const> entries) = 0;
        virtual void set_menu_source(HGDIOBJ ho, char const *indexed_bitmap) = 0;
        virtual void maybe_clear_menu_source(HGDIOBJ ho) = 0;
        virtual void present_menu_gdi() = 0;
        virtual void present_menu_surface_immediate() = 0;
        virtual void present_menu_surface_delayed() = 0;

        virtual void depth_clear_game() = 0;
        virtual void begin_game() = 0;
        virtual void execute_game(IDirect3DExecuteBuffer *cmdbuf, IDirect3DViewport *vp) = 0;
        virtual void end_game() = 0;
        virtual void present_game() = 0;

        virtual IDirectDraw *get_directdraw() = 0;
        virtual IDirectDraw2 *get_directdraw2() = 0;
        virtual IDirect3D *get_direct3d() = 0;
        virtual IDirect3DDevice *get_direct3ddevice() = 0;
        virtual IDirect3DViewport *get_direct3dviewport() = 0;

        virtual IDirect3DExecuteBuffer *create_direct3dexecutebuffer(size_t bufsz) = 0;

        virtual IDirectDrawSurface *get_directdraw_primary_surface() = 0;
        virtual IDirectDrawSurface *get_directdraw_backbuffer_surface() = 0;
        virtual IDirectDrawSurface *get_directdraw_backbuffer_menu_surface() = 0;
        virtual IDirectDrawSurface *get_directdraw_offscreen_surface(DDSURFACEDESC const &desc) = 0;
        virtual IDirectDrawSurface *get_directdraw_zbuffer_surface(DDSURFACEDESC const &desc) = 0;
        virtual IDirectDrawSurface *
            get_directdraw_sysmem_texture_surface(DDSURFACEDESC const &desc) = 0;
        virtual IDirectDrawSurface *
            get_directdraw_vidmem_texture_surface(DDSURFACEDESC const &desc) = 0;

        virtual IDirectDrawPalette *get_directdraw_palette(span<PALETTEENTRY const> entries) = 0;

        virtual std::optional<material const *> get_replacement_material(md5 const &sig) = 0;
    };

    std::unique_ptr<renderer> create_renderer(HINSTANCE dll_instance, config const *the_config);
}