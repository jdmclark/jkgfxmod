#pragma once

#include "renderer.hpp"
#include <d3d.h>
#include <vector>

namespace jkgm {
    class execute_buffer : public IDirect3DExecuteBuffer {
    public:
        int refct = 0;
        size_t bufsz = 0;
        D3DEXECUTEDATA exec_data;
        std::vector<char> buffer;

        explicit execute_buffer(size_t bufsz);

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