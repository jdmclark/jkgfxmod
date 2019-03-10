#pragma once

#include "ddrawsurface_impl.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    class zbuffer_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;

    public:
        DDSURFACEDESC sd;

        explicit zbuffer_surface(renderer *r);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
    };
}