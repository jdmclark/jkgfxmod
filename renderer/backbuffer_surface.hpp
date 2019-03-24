#pragma once

#include "ddrawsurface_impl.hpp"
#include "math/size.hpp"
#include "renderer_fwd.hpp"
#include <vector>

namespace jkgm {
    class backbuffer_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;

    public:
        std::vector<uint16_t> buffer;
        size<2, int> dims;

        uint16_t color_key = 0xF81FUL; // Magenta in RGB555

        backbuffer_surface(renderer *r, size<2, int> dims);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI AddAttachedSurface(LPDIRECTDRAWSURFACE a) override;
        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
    };
}