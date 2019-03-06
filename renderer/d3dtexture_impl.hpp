#pragma once

#include "renderer.hpp"
#include <d3d.h>

namespace jkgm {
    class Direct3DTexture_impl : public IDirect3DTexture {
    private:
        renderer *r;

    public:
        explicit Direct3DTexture_impl(renderer *r);

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