#include "ddraw_backbuffer_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDraw_backbuffer_surface_impl::DirectDraw_backbuffer_surface_impl(renderer *r)
    : r(r)
{
    // JK tries to lock the backbuffer to write raw data into it. Obviously this isn't something
    // that can reasonably happen today. Preallocate some reasonable size buffer to use as scratch
    // space to return to JK.
    scratch.resize(4096 * 4096 * 32);
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_WARNING("DirectDraw backbuffer surface::QueryInterface(", to_string(riid), ")");

    if(riid == ReusablePrimaryDirect3DDevice) {
        *ppvObj = r->get_direct3ddevice();
        return S_OK;
    }

    LOG_ERROR("DirectDraw backbuffer surface::QueryInterface not implemented for this type");
    abort();
}

ULONG WINAPI jkgm::DirectDraw_backbuffer_surface_impl::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw backbuffer surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw backbuffer surface::Release");
    return 1000;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_DEBUG("DirectDraw backbuffer surface::AddAttachedSurface call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::AddOverlayDirtyRect(LPRECT a)
{
    LOG_ERROR("DirectDraw backbuffer surface::AddOverlayDirtyRect unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Blt(LPRECT a,
                                                             LPDIRECTDRAWSURFACE b,
                                                             LPRECT c,
                                                             DWORD d,
                                                             LPDDBLTFX e)
{
    LOG_DEBUG("DirectDraw backbuffer surface::Blt call ignored");
    // Hack: Ignore this backbuffer blt for now. This is probably used for the HUD.
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
    LOG_ERROR("DirectDraw backbuffer surface::BltBatch unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::BltFast(DWORD a,
                                                                 DWORD b,
                                                                 LPDIRECTDRAWSURFACE c,
                                                                 LPRECT d,
                                                                 DWORD e)
{
    LOG_ERROR("DirectDraw backbuffer surface::BltFast unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_backbuffer_surface_impl::DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b)
{
    LOG_DEBUG("DirectDraw backbuffer surface::DeleteAttachedSurface call ignored");
    return DD_OK;
}

HRESULT WINAPI
    jkgm::DirectDraw_backbuffer_surface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                   LPDDENUMSURFACESCALLBACK b)
{
    LOG_ERROR("DirectDraw backbuffer surface::EnumAttachedSurfaces unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_backbuffer_surface_impl::EnumOverlayZOrders(DWORD a,
                                                                 LPVOID b,
                                                                 LPDDENUMSURFACESCALLBACK c)
{
    LOG_ERROR("DirectDraw backbuffer surface::EnumOverlayZOrders unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_ERROR("DirectDraw backbuffer surface::Flip unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetAttachedSurface(LPDDSCAPS a,
                                                                            LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetAttachedSurface(", a->dwCaps, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetBltStatus(DWORD a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetBltStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetCaps(LPDDSCAPS a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetDC(HDC *a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetFlipStatus(DWORD a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetFlipStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    LOG_ERROR("DirectDraw backbuffer surface::GetPixelFormat unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw backbuffer surface::GetSurfaceDesc(", a->dwFlags, ")");

    // Jedi Knight only asks for the surface caps and the number of back buffers
    a->dwBackBufferCount = 0;
    a->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Initialize(LPDIRECTDRAW a,
                                                                    LPDDSURFACEDESC b)
{
    LOG_ERROR("DirectDraw backbuffer surface::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::IsLost()
{
    LOG_ERROR("DirectDraw backbuffer surface::IsLost unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Lock(LPRECT a,
                                                              LPDDSURFACEDESC b,
                                                              DWORD c,
                                                              HANDLE d)
{
    LOG_ERROR("DirectDraw backbuffer surface::Lock(", c, ") call ignored");
    b->lpSurface = scratch.data();

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::ReleaseDC(HDC a)
{
    LOG_ERROR("DirectDraw backbuffer surface::ReleaseDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Restore()
{
    LOG_ERROR("DirectDraw backbuffer surface::Restore unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    LOG_ERROR("DirectDraw backbuffer surface::SetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw backbuffer surface::SetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::SetOverlayPosition(LONG a, LONG b)
{
    LOG_ERROR("DirectDraw backbuffer surface::SetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    LOG_DEBUG("DirectDraw backbuffer surface::SetPalette call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::Unlock(LPVOID a)
{
    LOG_ERROR("DirectDraw backbuffer surface::Unlock call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::UpdateOverlay(LPRECT a,
                                                                       LPDIRECTDRAWSURFACE b,
                                                                       LPRECT c,
                                                                       DWORD d,
                                                                       LPDDOVERLAYFX e)
{
    LOG_ERROR("DirectDraw backbuffer surface::UpdateOverlay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::UpdateOverlayDisplay(DWORD a)
{
    LOG_ERROR("DirectDraw backbuffer surface::UpdateOverlayDisplay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_backbuffer_surface_impl::UpdateOverlayZOrder(DWORD a,
                                                                             LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw backbuffer surface::UpdateOverlayZOrder unimplemented");
    abort();
}