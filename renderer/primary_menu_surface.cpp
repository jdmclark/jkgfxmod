#include "primary_menu_surface.hpp"
#include "backbuffer_menu_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"

jkgm::primary_menu_surface::primary_menu_surface(renderer *r)
    : DirectDrawSurface_impl("primary menu")
    , r(r)
{
    buffer.resize(640 * 480);

    ZeroMemory(&sd, sizeof(sd));
    sd.dwSize = sizeof(sd);

    sd.dwWidth = 640;
    sd.dwHeight = 480;
    sd.ddpfPixelFormat.dwRGBBitCount = 8;
    sd.lPitch = 640;
    sd.lpSurface = buffer.data();
}

ULONG WINAPI jkgm::primary_menu_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::primary_menu_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

void jkgm::primary_menu_surface::no_present_blt(LPRECT a,
                                                LPDIRECTDRAWSURFACE b,
                                                LPRECT c,
                                                DWORD d,
                                                LPDDBLTFX e)

{
    if(d & DDBLT_COLORFILL) {
        // Blit to clear region
        RECT real_a;

        real_a.left = 0;
        real_a.top = 0;
        real_a.right = sd.dwWidth;
        real_a.bottom = sd.dwHeight;

        if(a) {
            real_a.left = std::max(a->left, real_a.left);
            real_a.top = std::max(a->top, real_a.top);
            real_a.right = std::min(a->right, real_a.right);
            real_a.bottom = std::min(a->bottom, real_a.bottom);
        }

        // JK sets the color fill bit, which clears the destination area during every blt:
        auto clear_start = (sd.dwWidth * real_a.top) + real_a.left;
        for(int y = 0; y < (real_a.bottom - real_a.top); ++y) {
            for(int x = 0; x < (real_a.right - real_a.left); ++x) {
                buffer[clear_start + x] = (uint8_t)e->dwFillColor;
            }

            clear_start += sd.dwWidth;
        }
    }

    if(b) {
        // Copying B-frame data from the offscreen surface
        DDSURFACEDESC osd_sd;
        b->Lock(NULL, &osd_sd, 0, NULL);

        RECT real_a;
        RECT real_c;
        real_a.left = real_c.left = 0;
        real_a.top = real_c.top = 0;
        real_a.right = 640;
        real_a.bottom = 480;

        real_c.right = osd_sd.dwWidth;
        real_c.bottom = osd_sd.dwHeight;

        if(a) {
            real_a.left = std::max(real_a.left, a->left);
            real_a.top = std::max(real_a.top, a->top);
            real_a.right = std::min(real_a.right, a->right);
            real_a.bottom = std::min(real_a.bottom, a->bottom);
        }

        if(c) {
            real_c.left = std::max(real_c.left, c->left);
            real_c.top = std::max(real_c.top, c->top);
            real_c.right = std::min(real_c.right, c->right);
            real_c.bottom = std::min(real_c.bottom, c->bottom);
        }

        std::optional<uint8_t> colorkey;
        if(d & DDBLT_KEYSRC) {
            osd_sd.ddckCKSrcBlt.dwColorSpaceLowValue;
        }

        auto width_to_copy = std::min(a->right - a->left, c->right - c->left);
        auto height_to_copy = std::min(a->bottom - a->top, c->bottom - c->top);

        auto dest_start = (640 * a->top) + a->left;
        auto src_start = (osd_sd.dwWidth * c->top) + c->left;

        if(colorkey.has_value()) {
            for(int y = 0; y < height_to_copy; ++y) {
                for(int x = 0; x < width_to_copy; ++x) {
                    uint8_t src_col = ((uint8_t const *)osd_sd.lpSurface)[src_start + x];
                    if(src_col != *colorkey) {
                        buffer[dest_start + x] = src_col;
                    }
                }

                dest_start += 640;
                src_start += osd_sd.dwWidth;
            }
        }
        else {
            for(int y = 0; y < height_to_copy; ++y) {
                for(int x = 0; x < width_to_copy; ++x) {
                    uint8_t src_col = ((uint8_t const *)osd_sd.lpSurface)[src_start + x];
                    buffer[dest_start + x] = src_col;
                }

                dest_start += 640;
                src_start += osd_sd.dwWidth;
            }
        }

        b->Unlock(NULL);
    }
}

HRESULT WINAPI
    jkgm::primary_menu_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)

{
    no_present_blt(a, b, c, d, e);

    if(d & DDBLT_WAIT) {
        // Smacker blits to the primary surface to synchronize with the display.
        r->present_menu_surface_immediate();
    }
    else {
        r->present_menu_surface_delayed();
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    r->present_menu_surface_immediate();
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE *b)
{
    if(a->dwCaps == DDSCAPS_BACKBUFFER) {
        *b = r->get_directdraw_backbuffer_menu_surface();
        return DD_OK;
    }

    LOG_ERROR(
        "DirectDrawSurface(primary menu)::GetAttachedSurface(", a->dwCaps, ") unimplemented type");
    abort();
}

HRESULT WINAPI jkgm::primary_menu_surface::GetBltStatus(DWORD a)
{
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::GetFlipStatus(DWORD a)
{
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    *b = sd;
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::SetPalette(LPDIRECTDRAWPALETTE a)
{
    // Intentionally ignored: renderer tracks the global palette object
    return DD_OK;
}

HRESULT WINAPI jkgm::primary_menu_surface::Unlock(LPVOID a)
{
    // Present writes to the primary surface during the next scan.
    r->present_menu_surface_delayed();
    return DD_OK;
}