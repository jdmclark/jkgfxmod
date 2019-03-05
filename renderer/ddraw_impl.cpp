#include "ddraw_impl.hpp"
#include "base/log.hpp"

jkgm::DirectDraw_impl::DirectDraw_impl(IDirectDraw *orig)
    : orig(orig)
{
}

HRESULT WINAPI jkgm::DirectDraw_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("DirectDraw::QueryInterface");
    return orig->QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::DirectDraw_impl::AddRef()
{
    LOG_DEBUG("DirectDraw::AddRef");
    return orig->AddRef();
}

ULONG WINAPI jkgm::DirectDraw_impl::Release()
{
    LOG_DEBUG("DirectDraw::Release");
    return orig->Release();
}

HRESULT WINAPI jkgm::DirectDraw_impl::Compact()
{
    LOG_DEBUG("DirectDraw::Compact");
    return orig->Compact();
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER *b, IUnknown *c) {
    LOG_DEBUG("DirectDraw::CreateClipper");
    return orig->CreateClipper(a, b, c);
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreatePalette(DWORD a,
                                                    LPPALETTEENTRY b,
                                                    LPDIRECTDRAWPALETTE *c,
                                                    IUnknown *d)
{
    LOG_DEBUG("DirectDraw::CreatePalette");
    return orig->CreatePalette(a, b, c, d);
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreateSurface(LPDDSURFACEDESC a,
                                                    LPDIRECTDRAWSURFACE *b,
                                                    IUnknown *c)
{
    LOG_DEBUG("DirectDraw::CreateSurface");
    return orig->CreateSurface(a, b, c);
}

HRESULT WINAPI jkgm::DirectDraw_impl::DuplicateSurface(LPDIRECTDRAWSURFACE a,
                                                       LPDIRECTDRAWSURFACE *b)
{
    LOG_DEBUG("DirectDraw::DuplicateSurface");
    return orig->DuplicateSurface(a, b);
}

HRESULT WINAPI jkgm::DirectDraw_impl::EnumDisplayModes(DWORD a,
                                                       LPDDSURFACEDESC b,
                                                       LPVOID c,
                                                       LPDDENUMMODESCALLBACK d)
{
    LOG_DEBUG("DirectDraw::EnumDisplayModes");
    return orig->EnumDisplayModes(a, b, c, d);
}

HRESULT WINAPI jkgm::DirectDraw_impl::EnumSurfaces(DWORD a,
                                                   LPDDSURFACEDESC b,
                                                   LPVOID c,
                                                   LPDDENUMSURFACESCALLBACK d)
{
    LOG_DEBUG("DirectDraw::EnumSurfaces");
    return orig->EnumSurfaces(a, b, c, d);
}

HRESULT WINAPI jkgm::DirectDraw_impl::FlipToGDISurface()
{
    LOG_DEBUG("DirectDraw::FlipToGDISurface");
    return orig->FlipToGDISurface();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
    LOG_DEBUG("DirectDraw::GetCaps");
    return orig->GetCaps(a, b);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetDisplayMode(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw::GetDisplayMode");
    return orig->GetDisplayMode(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
    LOG_DEBUG("DirectDraw::GetFourCCCodes");
    return orig->GetFourCCCodes(a, b);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetGDISurface(LPDIRECTDRAWSURFACE *a)
{
    LOG_DEBUG("DirectDraw::GetGDISurface");
    return orig->GetGDISurface(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetMonitorFrequency(LPDWORD a)
{
    LOG_DEBUG("DirectDraw::GetMonitorFrequency");
    return orig->GetMonitorFrequency(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetScanLine(LPDWORD a)
{
    LOG_DEBUG("DirectDraw::GetScanLine");
    return orig->GetScanLine(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetVerticalBlankStatus(LPBOOL a)
{
    LOG_DEBUG("DirectDraw::GetVerticalBlankStatus");
    return orig->GetVerticalBlankStatus(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::Initialize(GUID *a)
{
    LOG_DEBUG("DirectDraw::Initialize");
    return orig->Initialize(a);
}

HRESULT WINAPI jkgm::DirectDraw_impl::RestoreDisplayMode()
{
    LOG_DEBUG("DirectDraw::RestoreDisplayMode");
    return orig->RestoreDisplayMode();
}

HRESULT WINAPI jkgm::DirectDraw_impl::SetCooperativeLevel(HWND a, DWORD b)
{
    LOG_DEBUG("DirectDraw::SetCooperativeLevel");
    return orig->SetCooperativeLevel(a, b);
}

HRESULT WINAPI jkgm::DirectDraw_impl::SetDisplayMode(DWORD a, DWORD b, DWORD c)
{
    LOG_DEBUG("DirectDraw::SetDisplayMode");
    return orig->SetDisplayMode(a, b, c);
}

HRESULT WINAPI jkgm::DirectDraw_impl::WaitForVerticalBlank(DWORD a, HANDLE b)
{
    LOG_DEBUG("DirectDraw::WaitForVerticalBlank");
    return orig->WaitForVerticalBlank(a, b);
}