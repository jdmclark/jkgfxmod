#include "backbuffer_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::backbuffer_surface::backbuffer_surface(renderer *r)
    : DirectDrawSurface_impl("backbuffer")
    , r(r)
{
    // JK tries to lock the backbuffer to write raw data into it. Obviously this isn't something
    // that can reasonably happen today. Preallocate some reasonable size buffer to use as scratch
    // space to return to JK.
    scratch.resize(4096 * 4096 * 32);
}

HRESULT WINAPI jkgm::backbuffer_surface::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == ReusablePrimaryDirect3DDevice) {
        *ppvObj = r->get_direct3ddevice();
        return S_OK;
    }

    return DirectDrawSurface_impl::QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::backbuffer_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(backbuffer)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::backbuffer_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawSurface(backbuffer)::Release");
    return 1000;
}

HRESULT WINAPI jkgm::backbuffer_surface::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_DEBUG("DirectDrawSurface(backbuffer)::AddAttachedSurface call ignored");
    return DD_OK;
}

HRESULT WINAPI
    jkgm::backbuffer_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    LOG_DEBUG("DirectDrawSurface(backbuffer)::Blt call ignored");
    // Hack: Ignore this backbuffer blt for now. This is probably used for the HUD.
    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    // Jedi Knight only asks for the surface caps and the number of back buffers
    a->dwBackBufferCount = 0;
    a->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;

    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    LOG_WARNING("DirectDrawSurface(backbuffer)::Lock(", c, ") call ignored");
    b->lpSurface = scratch.data();

    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::Unlock(LPVOID a)
{
    LOG_WARNING("DirectDrawSurface(backbuffer)::Unlock call ignored");
    return DD_OK;
}