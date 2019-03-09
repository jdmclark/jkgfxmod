#pragma once

#include "ddrawsurface_impl.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    class backbuffer_surface : public DirectDrawSurface_impl {
    private:
        renderer *r;
        std::vector<char> scratch;

    public:
        explicit backbuffer_surface(renderer *r);

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