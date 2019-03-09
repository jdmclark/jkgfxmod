#include "vidmem_texture.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "math/color_conv.hpp"
#include "sysmem_texture.hpp"

jkgm::vidmem_texture::vidmem_texture(renderer *r, vidmem_texture_surface *surf)
    : Direct3DTexture_impl("vidmem")
    , r(r)
    , surf(surf)
{
}

ULONG WINAPI jkgm::vidmem_texture::AddRef()
{
    // Direct3DTexture_vidmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(vidmem)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::vidmem_texture::Release()
{
    // Direct3DTexture_vidmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(vidmem)::Release");
    return 1000;
}

HRESULT WINAPI jkgm::vidmem_texture::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    LOG_DEBUG("Direct3DTexture(vidmem)::GetHandle");
    LOG_DEBUG("Self: ", (D3DTEXTUREHANDLE)(surf->texture_index));
    *b = (D3DTEXTUREHANDLE)(surf->texture_index);
    return D3D_OK;
}

HRESULT WINAPI jkgm::vidmem_texture::Load(LPDIRECT3DTEXTURE a)
{
    LOG_DEBUG("Direct3DTexture(vidmem)::Load");

    // Copy the input texture to the OpenGL surface
    auto *cast_tex = dynamic_cast<sysmem_texture *>(a);
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

jkgm::vidmem_texture_surface::vidmem_texture_surface(renderer *r,
                                                     DDSURFACEDESC desc,
                                                     size_t texture_index)
    : DirectDrawSurface_impl("vidmem texture")
    , r(r)
    , d3dtexture(r, this)
    , desc(desc)
    , texture_index(texture_index)
{
    if(desc.lpSurface != nullptr) {
        LOG_ERROR("JK IS PASSING A BUFFER TO A VIDMEM TEXTURE");
        abort();
    }

    // Construct some pessimistic backbuffer size
    buffer.resize(desc.dwWidth * desc.dwHeight, solid(color_rgb8::zero()));
}

HRESULT WINAPI jkgm::vidmem_texture_surface::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("DirectDraw vidmem texture surface::QueryInterface(", to_string(riid), ")");

    if(riid == IID_IDirect3DTexture) {
        *ppvObj = &d3dtexture;
        return S_OK;
    }

    return DirectDrawSurface_impl::QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::vidmem_texture_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw vidmem texture surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::vidmem_texture_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw vidmem texture surface::Release");
    return 1000;
}

HRESULT WINAPI jkgm::vidmem_texture_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw vidmem texture surface::GetSurfaceDesc(", a->dwFlags, ")");
    *a = desc;
    return DD_OK;
}

void jkgm::vidmem_texture_surface::regenerate_texture()
{
    gl::bind_texture(gl::texture_bind_target::texture_2d, ogl_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::rgba,
                     make_size((int)desc.dwWidth, (int)desc.dwHeight),
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::uint8,
                     make_span(buffer).as_const_bytes());
    gl::generate_mipmap(gl::texture_bind_target::texture_2d);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                               gl::min_filter::linear_mipmap_linear);
}