#include "ddrawsurface_impl.hpp"
#include "base/log.hpp"
#include "common/error_reporter.hpp"
#include "dxguids.hpp"

jkgm::DirectDrawSurface_impl::DirectDrawSurface_impl(char const *obj_name)
    : obj_name(obj_name)
{
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::QueryInterface(", to_string(riid), ")")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::AddAttachedSurface")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::AddOverlayDirtyRect(LPRECT a)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::AddOverlayDirtyRect")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Blt(LPRECT a,
                                                 LPDIRECTDRAWSURFACE b,
                                                 LPRECT c,
                                                 DWORD d,
                                                 LPDDBLTFX e)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Blt")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::BltBatch")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::BltFast(DWORD a,
                                                     DWORD b,
                                                     LPDIRECTDRAWSURFACE c,
                                                     LPRECT d,
                                                     DWORD e)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::BltFast")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::DeleteAttachedSurface")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                  LPDDENUMSURFACESCALLBACK b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::EnumAttachedSurfaces")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::EnumOverlayZOrders(DWORD a,
                                                                LPVOID b,
                                                                LPDDENUMSURFACESCALLBACK c)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::EnumOverlayZOrders")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Flip")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::GetAttachedSurface")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetBltStatus(DWORD a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetBltStatus")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetCaps(LPDDSCAPS a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetCaps")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetClipper")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetColorKey")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetDC(HDC *a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetDC")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetFlipStatus(DWORD a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetFlipStatus")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::GetOverlayPosition")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetPalette")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetPixelFormat")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::GetSurfaceDesc")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Initialize")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::IsLost()
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::IsLost")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Lock")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::ReleaseDC(HDC a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::ReleaseDC")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Restore()
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Restore")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::SetClipper")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::SetColorKey")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetOverlayPosition(LONG a, LONG b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::SetOverlayPosition")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::SetPalette")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::Unlock(LPVOID a)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::Unlock")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlay(LPRECT a,
                                                           LPDIRECTDRAWSURFACE b,
                                                           LPRECT c,
                                                           DWORD d,
                                                           LPDDOVERLAYFX e)
{
    report_unimplemented_function(str(fmt("DirectDrawSurface(", obj_name, ")::UpdateOverlay")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlayDisplay(DWORD a)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::UpdateOverlayDisplay")));
}

HRESULT WINAPI jkgm::DirectDrawSurface_impl::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b)
{
    report_unimplemented_function(
        str(fmt("DirectDrawSurface(", obj_name, ")::UpdateOverlayZOrder")));
}