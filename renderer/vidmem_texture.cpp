#include "vidmem_texture.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "math/color_conv.hpp"
#include "sysmem_texture.hpp"

jkgm::vidmem_texture::vidmem_texture(vidmem_texture_surface *surf)
    : Direct3DTexture_impl("vidmem")
    , surf(surf)
{
}

ULONG WINAPI jkgm::vidmem_texture::AddRef()
{
    return surf->AddRef();
}

ULONG WINAPI jkgm::vidmem_texture::Release()
{
    return surf->Release();
}

HRESULT WINAPI jkgm::vidmem_texture::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    *b = (D3DTEXTUREHANDLE)(surf->texture_index);
    return D3D_OK;
}

HRESULT WINAPI jkgm::vidmem_texture::Load(LPDIRECT3DTEXTURE a)
{
    // Copy the input texture to the OpenGL surface
    auto *cast_tex = dynamic_cast<sysmem_texture *>(a);
    if(!cast_tex) {
        LOG_ERROR("Direct3DTexture(vidmem)::Load passed a non-sysmem texture");
        abort();
    }

    auto *src = cast_tex->surf;

    uint16_t const *in_em = (uint16_t const *)src->buffer.data();

    for(auto &out_em : src->conv_buffer) {
        // Convert from indexed to RGB888
        if(src->desc.ddpfPixelFormat.dwRGBAlphaBitMask) {
            // Convert from RGBA5551 to RGBA8888
            out_em = rgba5551_to_srgb_a8(*in_em);
        }
        else {
            // Convert from RGB565 to RGBA8888
            out_em = rgb565_to_srgb_a8(*in_em);
        }

        ++in_em;
    }

    gl::bind_texture(gl::texture_bind_target::texture_2d, surf->ogl_texture);
    gl::tex_image_2d(gl::texture_bind_target::texture_2d,
                     /*level*/ 0,
                     gl::texture_internal_format::srgb_a8,
                     make_size((int)src->desc.dwWidth, (int)src->desc.dwHeight),
                     gl::texture_pixel_format::rgba,
                     gl::texture_pixel_type::uint8,
                     make_span(src->conv_buffer).as_const_bytes());
    gl::generate_mipmap(gl::texture_bind_target::texture_2d);
    gl::set_texture_mag_filter(gl::texture_bind_target::texture_2d, gl::mag_filter::linear);
    gl::set_texture_min_filter(gl::texture_bind_target::texture_2d,
                               gl::min_filter::linear_mipmap_linear);

    return D3D_OK;
}

jkgm::vidmem_texture_surface::vidmem_texture_surface(size_t texture_index)
    : DirectDrawSurface_impl("vidmem texture")
    , d3dtexture(this)
    , texture_index(texture_index)
{
}

void jkgm::vidmem_texture_surface::set_surface_desc(DDSURFACEDESC const &desc)
{
    // Basic sanity checking
    if(desc.lpSurface != nullptr) {
        LOG_ERROR("DirectDraw::CreateSurface(vidmem texture) call passes a buffer, which is not "
                  "implemented");
        abort();
    }

    this->desc = desc;
}

HRESULT WINAPI jkgm::vidmem_texture_surface::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == IID_IDirect3DTexture) {
        d3dtexture.AddRef();
        *ppvObj = &d3dtexture;
        return S_OK;
    }

    return DirectDrawSurface_impl::QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::vidmem_texture_surface::AddRef()
{
    return ++refct;
}

ULONG WINAPI jkgm::vidmem_texture_surface::Release()
{
    return --refct;
}

HRESULT WINAPI jkgm::vidmem_texture_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = desc;
    return DD_OK;
}