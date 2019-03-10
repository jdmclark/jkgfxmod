#include "ddraw2_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
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

    LOG_ERROR("Called unimplemented DirectDraw2::QueryInterface");
    abort();
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
    LOG_ERROR("DirectDraw2::Compact unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER *b, IUnknown *c)
{
    LOG_ERROR("DirectDraw2::CreateClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreatePalette(DWORD a,
                                                     LPPALETTEENTRY b,
                                                     LPDIRECTDRAWPALETTE *c,
                                                     IUnknown *d)
{
    LOG_ERROR("DirectDraw2::CreatePalette(", a, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::CreateSurface(LPDDSURFACEDESC a,
                                                     LPDIRECTDRAWSURFACE *b,
                                                     IUnknown *c)
{
    LOG_ERROR("DirectDraw2::CreateSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::DuplicateSurface(LPDIRECTDRAWSURFACE a,
                                                        LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR("DirectDraw2::DuplicateSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::EnumDisplayModes(DWORD a,
                                                        LPDDSURFACEDESC b,
                                                        LPVOID c,
                                                        LPDDENUMMODESCALLBACK d)
{
    LOG_ERROR("DirectDraw2::EnumDisplayModes(", a, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::EnumSurfaces(DWORD a,
                                                    LPDDSURFACEDESC b,
                                                    LPVOID c,
                                                    LPDDENUMSURFACESCALLBACK d)
{
    LOG_ERROR("DirectDraw2::EnumSurfaces unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::FlipToGDISurface()
{
    LOG_ERROR("DirectDraw2::FlipToGDISurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
    LOG_ERROR("DirectDraw2::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetDisplayMode(LPDDSURFACEDESC a)
{
    LOG_ERROR("DirectDraw2::GetDisplayMode unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
    LOG_ERROR("DirectDraw2::GetFourCCCodes unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetGDISurface(LPDIRECTDRAWSURFACE *a)
{
    LOG_ERROR("DirectDraw2::GetGDISurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetMonitorFrequency(LPDWORD a)
{
    LOG_ERROR("DirectDraw2::GetMonitorFrequency unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetScanLine(LPDWORD a)
{
    LOG_ERROR("DirectDraw2::GetScanLine unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetVerticalBlankStatus(LPBOOL a)
{
    LOG_ERROR("DirectDraw2::GetVerticalBlankStatus");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::Initialize(GUID *a)
{
    LOG_ERROR("DirectDraw2::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::RestoreDisplayMode()
{
    LOG_ERROR("DirectDraw2::RestoreDisplayMode unimplemented");
    abort();
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
        LOG_DEBUG("DirectDraw2::SetCooperativeLevel(Unknown ", b, ")");
        abort();
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw2_impl::SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
{
    LOG_ERROR(
        "DirectDraw2::SetDisplayMode(", a, ", ", b, ", ", c, ", ", d, ", ", e, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::WaitForVerticalBlank(DWORD a, HANDLE b)
{
    LOG_ERROR("DirectDraw2::WaitForVerticalBlank unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw2_impl::GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c)
{
    constexpr DWORD one_gb = 1 * 1024 * 1024 * 1024;
    *b = 1073741824;
    *c = 1073741824;
    return DD_OK;
}