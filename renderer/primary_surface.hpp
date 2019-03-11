#pragma once

#include "ddrawsurface_impl.hpp"
#include "renderer.hpp"

namespace jkgm {
    class primary_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;

    public:
        explicit primary_surface(renderer *r);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI Flip(LPDIRECTDRAWSURFACE a, DWORD b) override;
        HRESULT WINAPI GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b) override;
        HRESULT WINAPI GetBltStatus(DWORD a) override;
        HRESULT WINAPI GetFlipStatus(DWORD a) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
    };
}