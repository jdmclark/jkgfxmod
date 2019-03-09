#pragma once

#include "ddrawsurface_impl.hpp"
#include <vector>

namespace jkgm {
    class offscreen_surface : public DirectDrawSurface_impl {
    private:
        std::vector<char> buffer;

    public:
        DDSURFACEDESC sd;

        offscreen_surface();

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI SetColorKey(DWORD a, LPDDCOLORKEY b) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}