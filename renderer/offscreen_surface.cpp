#include "offscreen_surface.hpp"
#include "base/log.hpp"

jkgm::offscreen_surface::offscreen_surface()
    : DirectDrawSurface_impl("offscreen")
{
}

ULONG WINAPI jkgm::offscreen_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(offscreen)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::offscreen_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(offscreen)::Release");
    return 1000;
}

HRESULT WINAPI
    jkgm::offscreen_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    LOG_DEBUG("DirectDrawSurface(offscreen)::Blt call ignored");
    // Hack: Ignore this backbuffer blt for now. This is probably used for the HUD.
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    LOG_DEBUG("DirectDrawSurface(offscreen)::Lock call ignored");

    size_t needed_buffer_size = sd.dwWidth * sd.dwHeight * 4;
    if(buffer.size() < needed_buffer_size) {
        buffer.resize(needed_buffer_size);
    }

    *b = sd;
    b->lpSurface = buffer.data();
    b->dwFlags = b->dwFlags | DDSD_LPSURFACE;

    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_DEBUG("DirectDrawSurface(offscreen)::SetColorKey call ignored");
    // Hack:: Ignore this backbuffer color key for now. This is probably used for drawing the HUD.
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::Unlock(LPVOID a)
{
    LOG_DEBUG("DirectDrawSurface(offscreen)::Unlock call ignored");
    return DD_OK;
}