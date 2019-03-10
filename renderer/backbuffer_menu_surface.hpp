#pragma once

#include "ddrawsurface_impl.hpp"
#include "primary_menu_surface.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    class backbuffer_menu_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;
        primary_menu_surface *surf;

    public:
        backbuffer_menu_surface(renderer *r, primary_menu_surface *surf);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}