#pragma once

#include "renderer_fwd.hpp"
#include <d3d.h>

namespace jkgm {
    class Direct3D_impl : public IDirect3D {
    private:
        renderer *r;

    public:
        explicit Direct3D_impl(renderer *r);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Initialize(REFCLSID a) override;
        HRESULT WINAPI EnumDevices(LPD3DENUMDEVICESCALLBACK a, LPVOID b) override;
        HRESULT WINAPI CreateLight(LPDIRECT3DLIGHT *a, IUnknown *b) override;
        HRESULT WINAPI CreateMaterial(LPDIRECT3DMATERIAL *a, IUnknown *b) override;
        HRESULT WINAPI CreateViewport(LPDIRECT3DVIEWPORT *a, IUnknown *b) override;
        HRESULT WINAPI FindDevice(LPD3DFINDDEVICESEARCH a, LPD3DFINDDEVICERESULT b) override;
    };
}