#include "d3dtexture_vidmem_impl.hpp"
#include "base/log.hpp"
#include "ddraw_sysmem_texture_surface.hpp"
#include "ddraw_vidmem_texture_surface.hpp"
#include "dxguids.hpp"
#include "math/color_conv.hpp"

jkgm::Direct3DTexture_vidmem_impl::Direct3DTexture_vidmem_impl(
    renderer *r,
    DirectDraw_vidmem_texture_surface_impl *surf)
    : r(r)
    , surf(surf)
{
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DTexture(vidmem)::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3DTexture(vidmem)::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::Direct3DTexture_vidmem_impl::AddRef()
{
    // Direct3DTexture_vidmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(vidmem)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::Direct3DTexture_vidmem_impl::Release()
{
    // Direct3DTexture_vidmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(vidmem)::Release");
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::Initialize(LPDIRECT3DDEVICE a,
                                                             LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("Direct3DTexture(vidmem)::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::GetHandle(LPDIRECT3DDEVICE a,
                                                            LPD3DTEXTUREHANDLE b)
{
    LOG_DEBUG("Direct3DTexture(vidmem)::GetHandle");
    LOG_DEBUG("Self: ", (D3DTEXTUREHANDLE)(surf->texture_index));
    *b = (D3DTEXTUREHANDLE)(surf->texture_index);
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::PaletteChanged(DWORD a, DWORD b)
{
    LOG_ERROR("Direct3DTexture(vidmem)::PaletteChanged unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::Load(LPDIRECT3DTEXTURE a)
{
    LOG_DEBUG("Direct3DTexture(vidmem)::Load");

    // Copy the input texture to the OpenGL surface
    auto *cast_tex = dynamic_cast<Direct3DTexture_sysmem_impl *>(a);
    if(!cast_tex) {
        LOG_ERROR("Tried to load from a non-sysmem texture");
        abort();
    }

    uint16_t const *in_em = (uint16_t const *)cast_tex->surf->buffer.data();

    for(auto &out_em : surf->buffer) {
        // Convert from indexed to RGB888
        float r, g, b, a;
        if(surf->desc.ddpfPixelFormat.dwRGBAlphaBitMask) {
            // Convert from RGBA5551 to RGBA8888
            a = float((*in_em >> 15) & 0x1) / float(0x1);
            r = float((*in_em >> 10) & 0x1F) / float(0x1F);
            g = float((*in_em >> 5) & 0x1F) / float(0x1F);
            b = float((*in_em >> 0) & 0x1F) / float(0x1F);
        }
        else {
            // Convert from RGB565 to RGBA8888
            a = 1.0f;
            r = float((*in_em >> 11) & 0x1F) / float(0x1F);
            g = float((*in_em >> 5) & 0x3F) / float(0x3F);
            b = float((*in_em >> 0) & 0x1F) / float(0x1F);
        }

        out_em = to_discrete_color(srgb_to_linear(color(r * a, g * a, b * a, a)));
        ++in_em;
    }

    surf->regenerate_texture();

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DTexture_vidmem_impl::Unload()
{
    LOG_ERROR("Direct3DTexture(vidmem)::Unload unimplemented");
    abort();
}