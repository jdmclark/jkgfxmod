#pragma once

#include "renderer_fwd.hpp"
#include <d3d.h>

namespace jkgm {
    class Direct3DDevice_impl : public IDirect3DDevice {
    private:
        renderer *r;

    public:
        explicit Direct3DDevice_impl(renderer *r);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Initialize(LPDIRECT3D a, LPGUID b, LPD3DDEVICEDESC c) override;
        HRESULT WINAPI GetCaps(LPD3DDEVICEDESC, LPD3DDEVICEDESC) override;
        HRESULT WINAPI SwapTextureHandles(LPDIRECT3DTEXTURE a, LPDIRECT3DTEXTURE b) override;
        HRESULT WINAPI CreateExecuteBuffer(LPD3DEXECUTEBUFFERDESC a,
                                           LPDIRECT3DEXECUTEBUFFER *b,
                                           IUnknown *c) override;
        HRESULT WINAPI GetStats(LPD3DSTATS a) override;
        HRESULT WINAPI Execute(LPDIRECT3DEXECUTEBUFFER a, LPDIRECT3DVIEWPORT b, DWORD c) override;
        HRESULT WINAPI AddViewport(LPDIRECT3DVIEWPORT a) override;
        HRESULT WINAPI DeleteViewport(LPDIRECT3DVIEWPORT a) override;
        HRESULT WINAPI NextViewport(LPDIRECT3DVIEWPORT a, LPDIRECT3DVIEWPORT *b, DWORD c) override;
        HRESULT WINAPI Pick(LPDIRECT3DEXECUTEBUFFER a,
                            LPDIRECT3DVIEWPORT b,
                            DWORD c,
                            LPD3DRECT d) override;
        HRESULT WINAPI GetPickRecords(LPDWORD a, LPD3DPICKRECORD b) override;
        HRESULT WINAPI EnumTextureFormats(LPD3DENUMTEXTUREFORMATSCALLBACK a, LPVOID b) override;
        HRESULT WINAPI CreateMatrix(LPD3DMATRIXHANDLE a) override;
        HRESULT WINAPI SetMatrix(D3DMATRIXHANDLE a, const LPD3DMATRIX b) override;
        HRESULT WINAPI GetMatrix(D3DMATRIXHANDLE a, LPD3DMATRIX b) override;
        HRESULT WINAPI DeleteMatrix(D3DMATRIXHANDLE a) override;
        HRESULT WINAPI BeginScene() override;
        HRESULT WINAPI EndScene() override;
        HRESULT WINAPI GetDirect3D(LPDIRECT3D *a) override;
    };
}