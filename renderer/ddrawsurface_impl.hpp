#pragma once

#include <ddraw.h>

namespace jkgm {
    class DirectDrawSurface_impl : public IDirectDrawSurface {
    private:
        char const *obj_name;

    public:
        explicit DirectDrawSurface_impl(char const *obj_name);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;

        HRESULT WINAPI AddAttachedSurface(LPDIRECTDRAWSURFACE a) override;
        HRESULT WINAPI AddOverlayDirtyRect(LPRECT a) override;
        HRESULT WINAPI
            Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) override;
        HRESULT WINAPI BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c) override;
        HRESULT WINAPI BltFast(DWORD a, DWORD b, LPDIRECTDRAWSURFACE c, LPRECT d, DWORD e) override;
        HRESULT WINAPI DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b) override;
        HRESULT WINAPI EnumAttachedSurfaces(LPVOID a, LPDDENUMSURFACESCALLBACK b) override;
        HRESULT WINAPI EnumOverlayZOrders(DWORD a, LPVOID b, LPDDENUMSURFACESCALLBACK c) override;
        HRESULT WINAPI Flip(LPDIRECTDRAWSURFACE a, DWORD b) override;
        HRESULT WINAPI GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b) override;
        HRESULT WINAPI GetBltStatus(DWORD a) override;
        HRESULT WINAPI GetCaps(LPDDSCAPS a) override;
        HRESULT WINAPI GetClipper(LPDIRECTDRAWCLIPPER *a) override;
        HRESULT WINAPI GetColorKey(DWORD a, LPDDCOLORKEY b) override;
        HRESULT WINAPI GetDC(HDC *a) override;
        HRESULT WINAPI GetFlipStatus(DWORD a) override;
        HRESULT WINAPI GetOverlayPosition(LPLONG a, LPLONG b) override;
        HRESULT WINAPI GetPalette(LPDIRECTDRAWPALETTE *a) override;
        HRESULT WINAPI GetPixelFormat(LPDDPIXELFORMAT a) override;
        HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) override;
        HRESULT WINAPI Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b) override;
        HRESULT WINAPI IsLost() override;
        HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d) override;
        HRESULT WINAPI ReleaseDC(HDC a) override;
        HRESULT WINAPI Restore() override;
        HRESULT WINAPI SetClipper(LPDIRECTDRAWCLIPPER a) override;
        HRESULT WINAPI SetColorKey(DWORD a, LPDDCOLORKEY b) override;
        HRESULT WINAPI SetOverlayPosition(LONG a, LONG b) override;
        HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a) override;
        HRESULT WINAPI Unlock(LPVOID a) override;
        HRESULT WINAPI UpdateOverlay(LPRECT a,
                                     LPDIRECTDRAWSURFACE b,
                                     LPRECT c,
                                     DWORD d,
                                     LPDDOVERLAYFX e) override;
        HRESULT WINAPI UpdateOverlayDisplay(DWORD a) override;
        HRESULT WINAPI UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b) override;
    };
}