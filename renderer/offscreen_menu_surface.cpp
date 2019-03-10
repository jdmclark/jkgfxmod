#include "base/log.hpp"
#include "offscreen_menu_surface.hpp"

jkgm::offscreen_menu_surface::offscreen_menu_surface()
    : DirectDrawSurface_impl("offscreen menu")
{
}

void jkgm::offscreen_menu_surface::set_surface_desc(DDSURFACEDESC const &sd)
{
    this->sd = sd;
    size_t needed_buffer_size = sd.dwWidth * sd.dwHeight;
    if(buffer.size() < needed_buffer_size) {
        buffer.resize(needed_buffer_size);
    }

    this->sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    this->sd.ddpfPixelFormat.dwRGBBitCount = 8;
    this->sd.lPitch = sd.dwWidth;
}

ULONG WINAPI jkgm::offscreen_menu_surface::AddRef()
{
    // Offscreen surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::offscreen_menu_surface::Release()
{
    // Offscreen surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::offscreen_menu_surface::Blt(LPRECT a,
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
        real_a.right = sd.dwWidth;
        real_a.bottom = sd.dwHeight;

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

        auto width_to_copy = std::min(a->right - a->left, c->right - c->left);
        auto height_to_copy = std::min(a->bottom - a->top, c->bottom - c->top);

        auto dest_start = (sd.dwWidth * a->top) + a->left;
        auto src_start = (osd_sd.dwWidth * c->top) + c->left;
        for(int y = 0; y < height_to_copy; ++y) {
            for(int x = 0; x < width_to_copy; ++x) {
                uint8_t src_col = ((uint8_t const *)osd_sd.lpSurface)[src_start + x];
                buffer[dest_start + x] = src_col;
            }

            dest_start += sd.dwWidth;
            src_start += osd_sd.dwWidth;
        }

        b->Unlock(NULL);
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_menu_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_menu_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    *b = sd;
    b->lpSurface = buffer.data();
    b->dwFlags = b->dwFlags | DDSD_LPSURFACE;
    b->ddckCKSrcBlt.dwColorSpaceHighValue = color_key;
    b->ddckCKSrcBlt.dwColorSpaceLowValue = color_key;

    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_menu_surface::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    color_key = (uint8_t)b->dwColorSpaceLowValue;
    for(auto &em : buffer) {
        em = color_key;
    }
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_menu_surface::Unlock(LPVOID a)
{
    return DD_OK;
}