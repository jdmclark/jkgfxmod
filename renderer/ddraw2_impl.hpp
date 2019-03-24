#pragma once

#include "renderer_fwd.hpp"
#include <ddraw.h>

namespace jkgm {
    class DirectDraw2_impl : public IDirectDraw2 {
    private:
        renderer *r;

    public:
        explicit DirectDraw2_impl(renderer *r);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI Compact() override;
        HRESULT WINAPI CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER *b, IUnknown *c) override;
        HRESULT WINAPI CreatePalette(DWORD a,
                                     LPPALETTEENTRY b,
                                     LPDIRECTDRAWPALETTE *c,
                                     IUnknown *d) override;
        HRESULT WINAPI CreateSurface(LPDDSURFACEDESC a,
                                     LPDIRECTDRAWSURFACE *b,
                                     IUnknown *c) override;
        HRESULT WINAPI DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE *b) override;
        HRESULT WINAPI EnumDisplayModes(DWORD a,
                                        LPDDSURFACEDESC b,
                                        LPVOID c,
                                        LPDDENUMMODESCALLBACK d) override;
        HRESULT WINAPI EnumSurfaces(DWORD a,
                                    LPDDSURFACEDESC b,
                                    LPVOID c,
                                    LPDDENUMSURFACESCALLBACK d) override;
        HRESULT WINAPI FlipToGDISurface() override;
        HRESULT WINAPI GetCaps(LPDDCAPS a, LPDDCAPS b) override;
        HRESULT WINAPI GetDisplayMode(LPDDSURFACEDESC a) override;
        HRESULT WINAPI GetFourCCCodes(LPDWORD a, LPDWORD b) override;
        HRESULT WINAPI GetGDISurface(LPDIRECTDRAWSURFACE *a) override;
        HRESULT WINAPI GetMonitorFrequency(LPDWORD a) override;
        HRESULT WINAPI GetScanLine(LPDWORD a) override;
        HRESULT WINAPI GetVerticalBlankStatus(LPBOOL a) override;
        HRESULT WINAPI Initialize(GUID *a) override;
        HRESULT WINAPI RestoreDisplayMode() override;
        HRESULT WINAPI SetCooperativeLevel(HWND a, DWORD b) override;
        HRESULT WINAPI SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e) override;
        HRESULT WINAPI WaitForVerticalBlank(DWORD a, HANDLE b) override;
        HRESULT WINAPI GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c) override;
    };
}
#pragma once
