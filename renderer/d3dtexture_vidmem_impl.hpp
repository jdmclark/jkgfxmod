#pragma once

#include "renderer.hpp"
#include <d3d.h>

namespace jkgm {
    class DirectDraw_vidmem_texture_surface_impl;

    class Direct3DTexture_vidmem_impl : public IDirect3DTexture {
    private:
        renderer *r;
        DirectDraw_vidmem_texture_surface_impl *surf;

    public:
        Direct3DTexture_vidmem_impl(renderer *r, DirectDraw_vidmem_texture_surface_impl *surf);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Initialize(LPDIRECT3DDEVICE a, LPDIRECTDRAWSURFACE b) override;
        HRESULT WINAPI GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b) override;
        HRESULT WINAPI PaletteChanged(DWORD a, DWORD b) override;
        HRESULT WINAPI Load(LPDIRECT3DTEXTURE a) override;
        HRESULT WINAPI Unload() override;
    };
}