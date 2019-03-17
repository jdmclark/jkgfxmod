#pragma once

#include "common/material.hpp"
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
        vidmem_texture d3dtexture;

    public:
        renderer *r;

        int refct = 0;
        DDSURFACEDESC desc;

        material_instance_id material_id;

        // Vidmem textures embody a material instance:
        std::optional<srgb_texture_id> albedo_map;
        color albedo_factor = color::fill(1.0f);

        std::optional<srgb_texture_id> emissive_map;
        color_rgb emissive_factor = color_rgb::fill(0.0f);

        material_alpha_mode alpha_mode = material_alpha_mode::blend;
        float alpha_cutoff = 0.5f;

        vidmem_texture_surface(renderer *r, material_instance_id material_id);

        void clear();
        void set_surface_desc(DDSURFACEDESC const &desc);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
    };
}