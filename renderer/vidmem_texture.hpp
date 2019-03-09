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
        renderer *r;
        vidmem_texture_surface *surf;

    public:
        vidmem_texture(renderer *r, vidmem_texture_surface *surf);

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
        DDSURFACEDESC desc;
        std::vector<color_rgba8> buffer;

        size_t texture_index = 0U;
        gl::texture ogl_texture;

        vidmem_texture_surface(renderer *r, DDSURFACEDESC desc, size_t texture_index);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;

        void regenerate_texture();
    };
}