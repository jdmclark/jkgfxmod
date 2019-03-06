#pragma once

#include "renderer.hpp"
#include <d3d.h>
#include <vector>

namespace jkgm {
    class Direct3DExecuteBuffer_impl : public IDirect3DExecuteBuffer {
    private:
        renderer *r;
        size_t bufsz;
        std::vector<char> buffer;
        D3DEXECUTEDATA exec_data;

    public:
        Direct3DExecuteBuffer_impl(renderer *r, size_t bufsz);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Initialize(LPDIRECT3DDEVICE a, LPD3DEXECUTEBUFFERDESC b) override;
        HRESULT WINAPI Lock(LPD3DEXECUTEBUFFERDESC a) override;
        HRESULT WINAPI Unlock() override;
        HRESULT WINAPI SetExecuteData(LPD3DEXECUTEDATA a) override;
        HRESULT WINAPI GetExecuteData(LPD3DEXECUTEDATA a) override;
        HRESULT WINAPI Validate(LPDWORD a, LPD3DVALIDATECALLBACK b, LPVOID c, DWORD d) override;
        HRESULT WINAPI Optimize(DWORD a) override;
    };
}