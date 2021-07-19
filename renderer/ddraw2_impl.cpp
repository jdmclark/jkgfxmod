#include "ddraw2_impl.hpp"
#include "base/log.hpp"
#include "common/error_reporter.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"
#include <cassert>
#include <d3d.h>

jkgm::DirectDraw2_impl::DirectDraw2_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::DirectDraw2_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == IID_IDirect3D) {
        *ppvObj = r->get_direct3d();
        return S_OK;
    }

    report_unimplemented_function(str(fmt("DirectDraw2::QueryInterface(", to_string(riid), ")")));
}

ULONG WINAPI jkgm::DirectDraw2_impl::AddRef()
{
    // DirectDraw2 is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw2_impl::Release()
{
    // DirectDraw2 is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::DirectDraw2_impl::Compact()
{
    report_unimplemented_function("DirectDraw2::Compact");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER *b, IUnknown *c)
{
    report_unimplemented_function("DirectDraw2::CreateClipper");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreatePalette(DWORD a,
                                                     LPPALETTEENTRY b,
                                                     LPDIRECTDRAWPALETTE *c,
                                                     IUnknown *d)
{
    report_unimplemented_function("DirectDraw2::CreatePalette");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreateSurface(LPDDSURFACEDESC a,
                                                     LPDIRECTDRAWSURFACE *b,
                                                     IUnknown *c)
{
    report_unimplemented_function("DirectDraw2::CreateSurface");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::DuplicateSurface(LPDIRECTDRAWSURFACE a,
                                                        LPDIRECTDRAWSURFACE *b)
{
    report_unimplemented_function("DirectDraw2::DuplicateSurface");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::EnumDisplayModes(DWORD a,
                                                        LPDDSURFACEDESC b,
                                                        LPVOID c,
                                                        LPDDENUMMODESCALLBACK d)
{
    report_unimplemented_function("DirectDraw2::EnumDisplayModes");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::EnumSurfaces(DWORD a,
                                                    LPDDSURFACEDESC b,
                                                    LPVOID c,
                                                    LPDDENUMSURFACESCALLBACK d)
{
    report_unimplemented_function("DirectDraw2::EnumSurfaces");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::FlipToGDISurface()
{
    report_unimplemented_function("DirectDraw2::FlipToGDISurface");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
    report_unimplemented_function("DirectDraw2::GetCaps");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetDisplayMode(LPDDSURFACEDESC a)
{
    report_unimplemented_function("DirectDraw2::GetDisplayMode");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
    report_unimplemented_function("DirectDraw2::GetFourCCCodes");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetGDISurface(LPDIRECTDRAWSURFACE *a)
{
    report_unimplemented_function("DirectDraw2::GetGDISurface");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetMonitorFrequency(LPDWORD a)
{
    report_unimplemented_function("DirectDraw2::GetMonitorFrequency");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetScanLine(LPDWORD a)
{
    report_unimplemented_function("DirectDraw2::GetScanLine");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetVerticalBlankStatus(LPBOOL a)
{
    report_unimplemented_function("DirectDraw2::GetVerticalBlankStatus");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::Initialize(GUID *a)
{
    report_unimplemented_function("DirectDraw2::Initialize");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::RestoreDisplayMode()
{
    report_unimplemented_function("DirectDraw2::RestoreDisplayMode");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::SetCooperativeLevel(HWND a, DWORD b)
{
    if(((b & DDSCL_EXCLUSIVE) != 0) && ((b & DDSCL_FULLSCREEN) != 0)) {
        LOG_DEBUG("DirectDraw2::SetCooperativeLevel(Exclusive Fullscreen)");
    }
    else if(b == DDSCL_NORMAL) {
        LOG_DEBUG("DirectDraw2::SetCooperativeLevel(Normal)");
    }
    else {
        report_unimplemented_function(
            str(fmt("DirectDraw2::SetCooperativeLevel(Unknown ", b, ")")));
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw2_impl::SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
{
    report_unimplemented_function("DirectDraw2::SetDisplayMode");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::WaitForVerticalBlank(DWORD a, HANDLE b)
{
    report_unimplemented_function("DirectDraw2::WaitForVerticalBlank");
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c)
{
    constexpr DWORD one_gb = 1 * 1024 * 1024 * 1024;
    *b = 1073741824;
    *c = 1073741824;
    return DD_OK;
}