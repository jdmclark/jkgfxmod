#include "zbuffer_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"

jkgm::zbuffer_surface::zbuffer_surface(renderer *r)
    : DirectDrawSurface_impl("zbuffer")
    , r(r)
{
}

ULONG WINAPI jkgm::zbuffer_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::zbuffer_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI
    jkgm::zbuffer_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    // This is most likely an old hack used to clear the z-buffer.
    r->depth_clear_game();
    return DD_OK;
}

HRESULT WINAPI jkgm::zbuffer_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}