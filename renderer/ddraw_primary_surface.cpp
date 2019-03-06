#include "ddraw_primary_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDraw_primary_surface_impl::DirectDraw_primary_surface_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR("DirectDraw primary surface::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

ULONG WINAPI jkgm::DirectDraw_primary_surface_impl::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw primary surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw_primary_surface_impl::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw primary surface::Release");
    return 1000;
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_ERROR("DirectDraw primary surface::AddAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::AddOverlayDirtyRect(LPRECT a)
{
    LOG_ERROR("DirectDraw primary surface::AddOverlayDirtyRect unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Blt(LPRECT a,
                                                          LPDIRECTDRAWSURFACE b,
                                                          LPRECT c,
                                                          DWORD d,
                                                          LPDDBLTFX e)
{
    LOG_ERROR("DirectDraw primary surface::Blt unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
    LOG_ERROR("DirectDraw primary surface::BltBatch unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::BltFast(DWORD a,
                                                              DWORD b,
                                                              LPDIRECTDRAWSURFACE c,
                                                              LPRECT d,
                                                              DWORD e)
{
    LOG_ERROR("DirectDraw primary surface::BltFast unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::DeleteAttachedSurface(DWORD a,
                                                                            LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw primary surface::DeleteAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_primary_surface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                LPDDENUMSURFACESCALLBACK b)
{
    LOG_ERROR("DirectDraw primary surface::EnumAttachedSurfaces unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::EnumOverlayZOrders(DWORD a,
                                                                         LPVOID b,
                                                                         LPDDENUMSURFACESCALLBACK c)
{
    LOG_ERROR("DirectDraw primary surface::EnumOverlayZOrders unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_DEBUG("DirectDraw primary surface::Flip");
    r->present_game();
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetAttachedSurface(LPDDSCAPS a,
                                                                         LPDIRECTDRAWSURFACE *b)
{
    LOG_DEBUG("DirectDraw primary surface::GetAttachedSurface(", a->dwCaps, ")");

    if(a->dwCaps & DDSCAPS_BACKBUFFER) {
        *b = r->get_directdraw_backbuffer_surface();
        return DD_OK;
    }

    LOG_ERROR("DirectDraw primary surface::GetAttachedSurface unimplemented surface type");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetBltStatus(DWORD a)
{
    LOG_ERROR("DirectDraw primary surface::GetBltStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetCaps(LPDDSCAPS a)
{
    LOG_ERROR("DirectDraw primary surface::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    LOG_ERROR("DirectDraw primary surface::GetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw primary surface::GetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetDC(HDC *a)
{
    LOG_ERROR("DirectDraw primary surface::GetDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetFlipStatus(DWORD a)
{
    LOG_DEBUG("DirectDraw primary surface::GetFlipStatus call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    LOG_ERROR("DirectDraw primary surface::GetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    LOG_ERROR("DirectDraw primary surface::GetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    LOG_ERROR("DirectDraw primary surface::GetPixelFormat unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw primary surface::GetSurfaceDesc(", a->dwFlags, ")");

    // Jedi Knight only asks for the surface caps and the number of back buffers
    a->dwBackBufferCount = 1;
    a->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b)
{
    LOG_ERROR("DirectDraw primary surface::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::IsLost()
{
    LOG_ERROR("DirectDraw primary surface::IsLost unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Lock(LPRECT a,
                                                           LPDDSURFACEDESC b,
                                                           DWORD c,
                                                           HANDLE d)
{
    LOG_ERROR("DirectDraw primary surface::Lock unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::ReleaseDC(HDC a)
{
    LOG_ERROR("DirectDraw primary surface::ReleaseDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Restore()
{
    LOG_ERROR("DirectDraw primary surface::Restore unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    LOG_ERROR("DirectDraw primary surface::SetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw primary surface::SetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::SetOverlayPosition(LONG a, LONG b)
{
    LOG_ERROR("DirectDraw primary surface::SetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    LOG_DEBUG("DirectDraw primary surface::SetPalette call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::Unlock(LPVOID a)
{
    LOG_ERROR("DirectDraw primary surface::Unlock unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::UpdateOverlay(LPRECT a,
                                                                    LPDIRECTDRAWSURFACE b,
                                                                    LPRECT c,
                                                                    DWORD d,
                                                                    LPDDOVERLAYFX e)
{
    LOG_ERROR("DirectDraw primary surface::UpdateOverlay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::UpdateOverlayDisplay(DWORD a)
{
    LOG_ERROR("DirectDraw primary surface::UpdateOverlayDisplay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_primary_surface_impl::UpdateOverlayZOrder(DWORD a,
                                                                          LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw primary surface::UpdateOverlayZOrder unimplemented");
    abort();
}