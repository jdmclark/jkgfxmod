#include "ddraw_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"
#include <cassert>
#include <d3d.h>

jkgm::DirectDraw_impl::DirectDraw_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::DirectDraw_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == IID_IDirect3D) {
        *ppvObj = r->get_direct3d();
        return S_OK;
    }
    else if(riid == IID_IDirectDraw2) {
        *ppvObj = r->get_directdraw2();
        return S_OK;
    }

    LOG_ERROR("Called unimplemented DirectDraw::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::DirectDraw_impl::AddRef()
{
    // DirectDraw is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw_impl::Release()
{
    // DirectDraw is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::DirectDraw_impl::Compact()
{
    LOG_ERROR("DirectDraw::Compact unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER *b, IUnknown *c)
{
    LOG_ERROR("DirectDraw::CreateClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreatePalette(DWORD a,
                                                    LPPALETTEENTRY b,
                                                    LPDIRECTDRAWPALETTE *c,
                                                    IUnknown *d)
{
    LOG_ERROR("DirectDraw::CreatePalette(", a, ")");

    if(a & DDPCAPS_8BIT) {
        *c = r->get_directdraw_palette(make_span(b, 256));
        return DD_OK;
    }

    LOG_ERROR("DirectDraw::CreatePalette unimplemented palette type: ", a);
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::CreateSurface(LPDDSURFACEDESC a,
                                                    LPDIRECTDRAWSURFACE *b,
                                                    IUnknown *c)
{
    if(a->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) {
        *b = r->get_directdraw_primary_surface();
        return DD_OK;
    }
    else if(a->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) {
        *b = r->get_directdraw_offscreen_surface(*a);
        return DD_OK;
    }
    else if(a->ddsCaps.dwCaps & DDSCAPS_ZBUFFER) {
        *b = r->get_directdraw_zbuffer_surface(*a);
        return DD_OK;
    }
    else if(a->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
        if(a->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
            *b = r->get_directdraw_sysmem_texture_surface(*a);
            return DD_OK;
        }
        else if(a->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
            *b = r->get_directdraw_vidmem_texture_surface(*a);
            return DD_OK;
        }
    }

    LOG_ERROR("DirectDraw::CreateSurface(", a->ddsCaps.dwCaps, ") unimplemented for this type");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::DuplicateSurface(LPDIRECTDRAWSURFACE a,
                                                       LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR("DirectDraw::DuplicateSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::EnumDisplayModes(DWORD a,
                                                       LPDDSURFACEDESC b,
                                                       LPVOID c,
                                                       LPDDENUMMODESCALLBACK d)
{
    // Hack: Return just two display modes, the default 640x480 menu resolution and the desired game
    // resolution

    _DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.dwWidth = 640;
    ddsd.dwHeight = 480;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 8;

    d(&ddsd, c);

    ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE;
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    ddsd.ddpfPixelFormat.dwRBitMask = 0xF800ULL;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x7E0;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x1F;

    auto conf_scr_res = r->get_internal_screen_resolution();

    ddsd.dwWidth = get<x>(conf_scr_res);
    ddsd.dwHeight = get<y>(conf_scr_res);

    d(&ddsd, c);

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_impl::EnumSurfaces(DWORD a,
                                                   LPDDSURFACEDESC b,
                                                   LPVOID c,
                                                   LPDDENUMSURFACESCALLBACK d)
{
    LOG_ERROR("DirectDraw::EnumSurfaces unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::FlipToGDISurface()
{
    LOG_ERROR("DirectDraw::FlipToGDISurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
    // Hack: Return just the Direct3D hardware caps JK is looking for
    if(a) {
        a->dwCaps = DDCAPS_3D;
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetDisplayMode(LPDDSURFACEDESC a)
{
    LOG_ERROR("DirectDraw::GetDisplayMode unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
    LOG_ERROR("DirectDraw::GetFourCCCodes unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetGDISurface(LPDIRECTDRAWSURFACE *a)
{
    LOG_ERROR("DirectDraw::GetGDISurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetMonitorFrequency(LPDWORD a)
{
    LOG_ERROR("DirectDraw::GetMonitorFrequency unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetScanLine(LPDWORD a)
{
    LOG_ERROR("DirectDraw::GetScanLine unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::GetVerticalBlankStatus(LPBOOL a)
{
    LOG_ERROR("DirectDraw::GetVerticalBlankStatus");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::Initialize(GUID *a)
{
    LOG_ERROR("DirectDraw::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::RestoreDisplayMode()
{
    LOG_ERROR("DirectDraw::RestoreDisplayMode unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_impl::SetCooperativeLevel(HWND a, DWORD b)
{
    if(((b & DDSCL_EXCLUSIVE) != 0) && ((b & DDSCL_FULLSCREEN) != 0)) {
        LOG_DEBUG("DirectDraw::SetCooperativeLevel(Exclusive Fullscreen)");
    }
    else if(b == DDSCL_NORMAL) {
        LOG_DEBUG("DirectDraw::SetCooperativeLevel(Normal)");
    }
    else {
        LOG_ERROR("DirectDraw::SetCooperativeLevel(Unknown ", b, ")");
        abort();
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_impl::SetDisplayMode(DWORD a, DWORD b, DWORD c)
{
    if(a == 640 && b == 480 && c == 8) {
        r->set_renderer_mode(renderer_mode::menu);
    }
    else {
        r->set_renderer_mode(renderer_mode::ingame);
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_impl::WaitForVerticalBlank(DWORD a, HANDLE b)
{
    r->present_menu_surface_immediate();
    return DD_OK;
}