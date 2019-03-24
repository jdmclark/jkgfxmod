#include "backbuffer_menu_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "primary_menu_surface.hpp"
#include "renderer.hpp"

jkgm::backbuffer_menu_surface::backbuffer_menu_surface(renderer *r, primary_menu_surface *surf)
    : DirectDrawSurface_impl("backbuffer menu")
    , r(r)
    , surf(surf)
{
}

ULONG WINAPI jkgm::backbuffer_menu_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::backbuffer_menu_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::backbuffer_menu_surface::Blt(LPRECT a,
                                                  LPDIRECTDRAWSURFACE b,
                                                  LPRECT c,
                                                  DWORD d,
                                                  LPDDBLTFX e)
{
    surf->no_present_blt(a, b, c, d, e);
    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_menu_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    return surf->GetSurfaceDesc(a);
}

HRESULT WINAPI jkgm::backbuffer_menu_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    return surf->Lock(a, b, c, d);
}

HRESULT WINAPI jkgm::backbuffer_menu_surface::SetPalette(LPDIRECTDRAWPALETTE a)
{
    // Intentionally ignored: renderer tracks the global palette object
    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_menu_surface::Unlock(LPVOID a)
{
    return DD_OK;
}