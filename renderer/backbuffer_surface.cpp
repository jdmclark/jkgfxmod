#include "backbuffer_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "offscreen_surface.hpp"
#include "renderer.hpp"

jkgm::backbuffer_surface::backbuffer_surface(renderer *r, size<2, int> dims)
    : DirectDrawSurface_impl("backbuffer")
    , r(r)
    , dims(dims)
{
    buffer.resize(get<x>(dims) * get<y>(dims));
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
    return 1000;
}

ULONG WINAPI jkgm::backbuffer_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::backbuffer_surface::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    // This function is called to attach the Z-buffer. The renderer manages its own z-buffer, so
    // this call is intentionally ignored.
    return DD_OK;
}

HRESULT WINAPI
    jkgm::backbuffer_surface::Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e)
{
    if(b == NULL) {
        // This is most likely an old hack to clear the backbuffer
        for(auto &em : buffer) {
            em = (uint16_t)color_key;
        }

        return DD_OK;
    }

    auto *osd = dynamic_cast<offscreen_surface *>(b);
    if(osd && a && c) {
        // This is copying the console text to the top of the screen

        // Clear the area
        auto clear_start = (get<x>(dims) * a->top) + a->left;
        for(int y = 0; y < (a->bottom - a->top); ++y) {
            for(int x = 0; x < (a->right - a->left); ++x) {
                buffer[clear_start + x] = color_key;
            }

            clear_start += get<x>(dims);
        }

        auto width_to_copy = std::min(a->right - a->left, c->right - c->left);
        auto height_to_copy = std::min(a->bottom - a->top, c->bottom - c->top);

        auto dest_start = (get<x>(dims) * a->top) + a->left;
        auto src_start = (osd->sd.dwWidth * c->top) + c->left;
        for(int y = 0; y < height_to_copy; ++y) {
            for(int x = 0; x < width_to_copy; ++x) {
                auto src_col = osd->buffer[src_start + x];
                if(src_col != osd->color_key) {
                    buffer[dest_start + x] = src_col;
                }
            }

            dest_start += get<x>(dims);
            src_start += osd->sd.dwWidth;
        }
    }

    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    // Jedi Knight only asks for the surface caps and the number of back buffers
    a->dwBackBufferCount = 0;
    a->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
    a->ddpfPixelFormat.dwFlags = DDPF_RGB;
    a->ddpfPixelFormat.dwRGBBitCount = 16;
    a->ddpfPixelFormat.dwBBitMask = 0x0000001F;
    a->ddpfPixelFormat.dwGBitMask = 0x000007E0;
    a->ddpfPixelFormat.dwRBitMask = 0x0000F800;
    a->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
    a->lPitch = get<x>(dims) * 2;
    a->dwWidth = get<x>(dims);
    a->dwHeight = get<y>(dims);

    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    b->lpSurface = buffer.data();

    return DD_OK;
}

HRESULT WINAPI jkgm::backbuffer_surface::Unlock(LPVOID a)
{
    return DD_OK;
}