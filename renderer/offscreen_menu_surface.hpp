#pragma once

#include "ddrawsurface_impl.hpp"
#include <vector>

namespace jkgm {
    class offscreen_menu_surface : public DirectDrawSurface_impl {
    public:
        DDSURFACEDESC sd;
        std::vector<uint8_t> buffer;

        offscreen_menu_surface();

        void set_surface_desc(DDSURFACEDESC const &sd);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}