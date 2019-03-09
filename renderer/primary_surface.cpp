#include "primary_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::primary_surface::primary_surface(renderer *r)
    : DirectDrawSurface_impl("primary")
    , r(r)
{
}

ULONG WINAPI jkgm::primary_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw primary surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::primary_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw primary surface::Release");
    return 1000;
}

HRESULT WINAPI jkgm::primary_surface::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_DEBUG("DirectDraw primary surface::Flip ignored");
    // HACK: Implement
    // r->present_game();
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_surface::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b)
{
    if(a->dwCaps & DDSCAPS_BACKBUFFER) {
        *b = r->get_directdraw_backbuffer_surface();
        return DD_OK;
    }

    LOG_ERROR("DirectDraw primary surface::GetAttachedSurface unimplemented surface type");
    abort();
}

HRESULT WINAPI jkgm::primary_surface::GetFlipStatus(DWORD a)
{
    LOG_DEBUG("DirectDraw primary surface::GetFlipStatus call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw primary surface::GetSurfaceDesc(", a->dwFlags, ")");

    // Jedi Knight only asks for the surface caps and the number of back buffers
    a->dwBackBufferCount = 1;
    a->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

    return DD_OK;
}