#include "zbuffer_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::zbuffer_surface::zbuffer_surface()
    : DirectDrawSurface_impl("zbuffer")
{
}

ULONG WINAPI jkgm::zbuffer_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(zbuffer)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::zbuffer_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(zbuffer)::Release");
    return 1000;
}

HRESULT WINAPI
    jkgm::zbuffer_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    LOG_DEBUG("DirectDrawSurface(zbuffer)::Blt call ignored");
    // Hack: This is most likely a hack used to clear the zbuffer. Ignore for now.
    return DD_OK;
}

HRESULT WINAPI jkgm::zbuffer_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}