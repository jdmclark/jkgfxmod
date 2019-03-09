#include "ddrawsurface_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDrawSurface_impl::DirectDrawSurface_impl(char const *obj_name)
    : obj_name(obj_name)
{
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR(
        "DirectDrawSurface(", obj_name, ")::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::AddAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::AddOverlayDirtyRect(LPRECT a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::AddOverlayDirtyRect unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Blt(LPRECT a,
                                                 LPDIRECTDRAWSURFACE b,
                                                 LPRECT c,
                                                 DWORD d,
                                                 LPDDBLTFX e)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Blt unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::BltBatch unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::BltFast(DWORD a,
                                                     DWORD b,
                                                     LPDIRECTDRAWSURFACE c,
                                                     LPRECT d,
                                                     DWORD e)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::BltFast unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::DeleteAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                  LPDDENUMSURFACESCALLBACK b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::EnumAttachedSurfaces unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::EnumOverlayZOrders(DWORD a,
                                                                LPVOID b,
                                                                LPDDENUMSURFACESCALLBACK c)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::EnumOverlayZOrders unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Flip unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetBltStatus(DWORD a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetBltStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetCaps(LPDDSCAPS a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetDC(HDC *a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetFlipStatus(DWORD a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetFlipStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetPixelFormat unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::GetSurfaceDesc unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::IsLost()
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::IsLost unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Lock unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::ReleaseDC(HDC a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::ReleaseDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Restore()
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Restore unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::SetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::SetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetOverlayPosition(LONG a, LONG b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::SetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::SetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Unlock(LPVOID a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::Unlock unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlay(LPRECT a,
                                                           LPDIRECTDRAWSURFACE b,
                                                           LPRECT c,
                                                           DWORD d,
                                                           LPDDOVERLAYFX e)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::UpdateOverlay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlayDisplay(DWORD a)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::UpdateOverlayDisplay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDrawSurface(", obj_name, ")::UpdateOverlayZOrder unimplemented");
    abort();
}