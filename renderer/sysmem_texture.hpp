#pragma once

#include "d3dtexture_impl.hpp"
#include "ddrawsurface_impl.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    class sysmem_texture_surface;

    class sysmem_texture : public Direct3DTexture_impl {
    private:
        renderer *r;

    public:
        sysmem_texture_surface *surf;

        sysmem_texture(renderer *r, sysmem_texture_surface *surf);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;
    };

    class sysmem_texture_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;
        sysmem_texture d3dtexture;

        DDSURFACEDESC desc;

    public:
        std::vector<char> buffer;

        sysmem_texture_surface(renderer *r, DDSURFACEDESC desc);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}
