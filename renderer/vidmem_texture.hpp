#pragma once

#include "d3dtexture_impl.hpp"
#include "ddrawsurface_impl.hpp"
#include "glutil/texture.hpp"
#include "renderer.hpp"
#include <d3d.h>
#include <ddraw.h>
#include <vector>

namespace jkgm {
    class vidmem_texture_surface;

    class vidmem_texture : public Direct3DTexture_impl {
    private:
        vidmem_texture_surface *surf;

    public:
        explicit vidmem_texture(vidmem_texture_surface *surf);

        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b) override;
        HRESULT WINAPI Load(LPDIRECT3DTEXTURE a) override;
    };

    class vidmem_texture_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;
        vidmem_texture d3dtexture;

    public:
        int refct = 0;
        DDSURFACEDESC desc;

        size_t texture_index = 0U;
        gl::texture ogl_texture;

        explicit vidmem_texture_surface(size_t texture_index);

        void set_surface_desc(DDSURFACEDESC const &desc);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
    };
}