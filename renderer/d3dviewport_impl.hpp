#pragma once

#include "renderer.hpp"
#include <d3d.h>

namespace jkgm {
    class Direct3DViewport_impl : public IDirect3DViewport {
    private:
        renderer *r;
        D3DVIEWPORT data;

    public:
        explicit Direct3DViewport_impl(renderer *r);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Initialize(LPDIRECT3D a) override;
        HRESULT WINAPI GetViewport(LPD3DVIEWPORT a) override;
        HRESULT WINAPI SetViewport(LPD3DVIEWPORT a) override;
        HRESULT WINAPI TransformVertices(DWORD a,
                                         LPD3DTRANSFORMDATA b,
                                         DWORD c,
                                         LPDWORD d) override;
        HRESULT WINAPI LightElements(DWORD a, LPD3DLIGHTDATA b) override;
        HRESULT WINAPI SetBackground(D3DMATERIALHANDLE a) override;
        HRESULT WINAPI GetBackground(LPD3DMATERIALHANDLE a, LPBOOL b) override;
        HRESULT WINAPI SetBackgroundDepth(LPDIRECTDRAWSURFACE a) override;
        HRESULT WINAPI GetBackgroundDepth(LPDIRECTDRAWSURFACE *a, LPBOOL b) override;
        HRESULT WINAPI Clear(DWORD a, LPD3DRECT b, DWORD c) override;
        HRESULT WINAPI AddLight(LPDIRECT3DLIGHT a) override;
        HRESULT WINAPI DeleteLight(LPDIRECT3DLIGHT a) override;
        HRESULT WINAPI NextLight(LPDIRECT3DLIGHT a, LPDIRECT3DLIGHT *b, DWORD c) override;
    };
}