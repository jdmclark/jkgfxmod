#pragma once

#include "d3dtexture_impl.hpp"
#include "ddrawsurface_impl.hpp"
#include "math/color.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    class sysmem_texture_surface;

    class sysmem_texture : public Direct3DTexture_impl {
    public:
        sysmem_texture_surface *surf;

        explicit sysmem_texture(sysmem_texture_surface *surf);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;
    };

    class sysmem_texture_surface : public DirectDrawSurface_impl {
    private:
        sysmem_texture d3dtexture;

    public:
        int refct = 0;
        size_t num_pixels;

        DDSURFACEDESC desc;
        std::vector<char> buffer;
        std::vector<color_rgba8> conv_buffer;

        explicit sysmem_texture_surface(size_t num_pixels);

        void set_surface_desc(DDSURFACEDESC const &desc);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}
