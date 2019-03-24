#pragma once

#include "ddrawsurface_impl.hpp"
#include "renderer_fwd.hpp"
#include <vector>

namespace jkgm {
    class primary_menu_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;
        DDSURFACEDESC sd;

    public:
        std::vector<uint8_t> buffer;

        explicit primary_menu_surface(renderer *r);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        void no_present_blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e);

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI Flip(LPDIRECTDRAWSURFACE a, DWORD b) override;
        HRESULT WINAPI GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b) override;
        HRESULT WINAPI GetBltStatus(DWORD a) override;
        HRESULT WINAPI GetFlipStatus(DWORD a) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}