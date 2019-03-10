#include "offscreen_surface.hpp"
#include "base/log.hpp"

jkgm::offscreen_surface::offscreen_surface()
    : DirectDrawSurface_impl("offscreen")
{
}

void jkgm::offscreen_surface::set_surface_desc(DDSURFACEDESC const &sd)
{
    this->sd = sd;
    size_t needed_buffer_size = sd.dwWidth * sd.dwHeight;
    if(buffer.size() < needed_buffer_size) {
        buffer.resize(needed_buffer_size);
        dblbuffer.resize(needed_buffer_size);
    }

    this->sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    this->sd.ddpfPixelFormat.dwRGBBitCount = 16;
    this->sd.ddpfPixelFormat.dwBBitMask = 0x0000001F;
    this->sd.ddpfPixelFormat.dwGBitMask = 0x000007E0;
    this->sd.ddpfPixelFormat.dwRBitMask = 0x0000F800;
    this->sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
    this->sd.lPitch = sd.dwWidth * 2;
}

ULONG WINAPI jkgm::offscreen_surface::AddRef()
{
    // Offscreen surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::offscreen_surface::Release()
{
    // Offscreen surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI
    jkgm::offscreen_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    // Blit from self onto self
    RECT real_a;
    RECT real_c;

    real_a.left = real_c.left = 0;
    real_a.top = real_c.top = 0;
    real_a.right = real_c.right = sd.dwWidth;
    real_a.bottom = real_c.bottom = sd.dwHeight;

    LOG_DEBUG("OFFSCREEN BLT: ", (DWORD)a, " ", (DWORD)b, " ", (DWORD)c, " ", d);

    if(a) {
        real_a.left = std::max(a->left, real_a.left);
        real_a.top = std::max(a->top, real_a.top);
        real_a.right = std::min(a->right, real_a.right);
        real_a.bottom = std::min(a->bottom, real_a.bottom);
    }

    if(c) {
        real_c.left = std::max(c->left, real_c.left);
        real_c.top = std::max(c->top, real_c.top);
        real_c.right = std::min(c->right, real_c.right);
        real_c.bottom = std::min(c->bottom, real_c.bottom);
    }

    std::copy(buffer.begin(), buffer.end(), dblbuffer.begin());

    // JK sets the color fill bit, which clears the destination area during every blt:
    auto clear_start = (sd.dwWidth * real_a.top) + real_a.left;
    for(int y = 0; y < (real_a.bottom - real_a.top); ++y) {
        for(int x = 0; x < (real_a.right - real_a.left); ++x) {
            buffer[clear_start + x] = color_key;
        }

        clear_start += sd.dwWidth;
    }

    auto width_to_copy = std::min(real_a.right - real_a.left, real_c.right - real_c.left);
    auto height_to_copy = std::min(real_a.bottom - real_a.top, real_c.bottom - real_c.top);

    auto dest_start = (sd.dwWidth * real_a.top) + real_a.left;
    auto src_start = (sd.dwWidth * real_c.top) + real_c.left;
    for(int y = 0; y < height_to_copy; ++y) {
        for(int x = 0; x < width_to_copy; ++x) {
            auto src_col = dblbuffer[src_start + x];
            buffer[dest_start + x] = src_col;
        }

        dest_start += sd.dwWidth;
        src_start += sd.dwWidth;
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = sd;
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    *b = sd;
    b->lpSurface = buffer.data();
    b->dwFlags = b->dwFlags | DDSD_LPSURFACE;

    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    color_key = (uint16_t)b->dwColorSpaceLowValue;
    for(auto &em : buffer) {
        em = color_key;
    }
    return DD_OK;
}

HRESULT WINAPI jkgm::offscreen_surface::Unlock(LPVOID a)
{
    return DD_OK;
}