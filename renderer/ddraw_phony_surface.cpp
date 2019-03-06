#include "ddraw_phony_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDraw_phony_surface_impl::DirectDraw_phony_surface_impl(renderer *r, DDSURFACEDESC desc)
    : r(r)
    , desc(desc)
{
    // Construct some pessimistic backbuffer size
    buffer.resize(desc.dwWidth * desc.dwHeight * 4 * 32);
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("DirectDraw phony surface::QueryInterface(", to_string(riid), ")");

    if(riid == IID_IDirect3DTexture) {
        *ppvObj = r->get_direct3dtexture();
        return S_OK;
    }

    LOG_ERROR("Called unimplemented DirectDraw phony surface::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::DirectDraw_phony_surface_impl::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw phony surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw_phony_surface_impl::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw phony surface::Release");
    return 1000;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_ERROR("DirectDraw phony surface::AddAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::AddOverlayDirtyRect(LPRECT a)
{
    LOG_ERROR("DirectDraw phony surface::AddOverlayDirtyRect unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Blt(LPRECT a,
                                                        LPDIRECTDRAWSURFACE b,
                                                        LPRECT c,
                                                        DWORD d,
                                                        LPDDBLTFX e)
{
    LOG_DEBUG("DirectDraw phony surface::Blt call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
    LOG_DEBUG("DirectDraw phony surface::BltBatch ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::BltFast(DWORD a,
                                                            DWORD b,
                                                            LPDIRECTDRAWSURFACE c,
                                                            LPRECT d,
                                                            DWORD e)
{
    LOG_DEBUG("DirectDraw phony surface::BltFast ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::DeleteAttachedSurface(DWORD a,
                                                                          LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw phony surface::DeleteAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                         LPDDENUMSURFACESCALLBACK b)
{
    LOG_ERROR("DirectDraw phony surface::EnumAttachedSurfaces unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::EnumOverlayZOrders(DWORD a,
                                                                       LPVOID b,
                                                                       LPDDENUMSURFACESCALLBACK c)
{
    LOG_ERROR("DirectDraw phony surface::EnumOverlayZOrders unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_ERROR("DirectDraw phony surface::Flip unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetAttachedSurface(LPDDSCAPS a,
                                                                       LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR("DirectDraw phony surface::GetAttachedSurface(", a->dwCaps, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetBltStatus(DWORD a)
{
    LOG_ERROR("DirectDraw phony surface::GetBltStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetCaps(LPDDSCAPS a)
{
    LOG_ERROR("DirectDraw phony surface::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    LOG_ERROR("DirectDraw phony surface::GetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw phony surface::GetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetDC(HDC *a)
{
    LOG_ERROR("DirectDraw phony surface::GetDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetFlipStatus(DWORD a)
{
    LOG_ERROR("DirectDraw phony surface::GetFlipStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    LOG_ERROR("DirectDraw phony surface::GetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    LOG_ERROR("DirectDraw phony surface::GetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    LOG_ERROR("DirectDraw phony surface::GetPixelFormat unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw phony surface::GetSurfaceDesc(", a->dwFlags, ")");
    LOG_DEBUG("Desc size: ", desc.dwSize, " dest size: ", a->dwSize);
    *a = desc;
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b)
{
    LOG_ERROR("DirectDraw phony surface::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::IsLost()
{
    LOG_ERROR("DirectDraw phony surface::IsLost unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Lock(LPRECT a,
                                                         LPDDSURFACEDESC b,
                                                         DWORD c,
                                                         HANDLE d)
{
    LOG_DEBUG("DirectDraw phony surface::Lock ignored");
    // TODO: This is how texture data is uploaded. Implement.
    LOG_DEBUG("Desc size: ", desc.dwSize, " dest size: ", b->dwSize);
    *b = desc;
    b->lpSurface = buffer.data();
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::ReleaseDC(HDC a)
{
    LOG_ERROR("DirectDraw phony surface::ReleaseDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Restore()
{
    LOG_ERROR("DirectDraw phony surface::Restore unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    LOG_ERROR("DirectDraw phony surface::SetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_DEBUG("DirectDraw phony surface::SetColorKey call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::SetOverlayPosition(LONG a, LONG b)
{
    LOG_ERROR("DirectDraw phony surface::SetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    LOG_DEBUG("DirectDraw phony surface::SetPalette call ignored");
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::Unlock(LPVOID a)
{
    LOG_DEBUG("DirectDraw phony surface::Unlock ignored");
    // TODO: This is how texture data is uploaded. Implement.
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::UpdateOverlay(LPRECT a,
                                                                  LPDIRECTDRAWSURFACE b,
                                                                  LPRECT c,
                                                                  DWORD d,
                                                                  LPDDOVERLAYFX e)
{
    LOG_ERROR("DirectDraw phony surface::UpdateOverlay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::UpdateOverlayDisplay(DWORD a)
{
    LOG_ERROR("DirectDraw phony surface::UpdateOverlayDisplay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_phony_surface_impl::UpdateOverlayZOrder(DWORD a,
                                                                        LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw phony surface::UpdateOverlayZOrder unimplemented");
    abort();
}