#include "vidmem_texture.hpp"
#include "base/file_block.hpp"
#include "base/log.hpp"
#include "base/md5.hpp"
#include "common/image.hpp"
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
    *b = (D3DTEXTUREHANDLE)(surf->material_id.get());
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

    // Reset material to default state
    surf->clear();

    auto *src = cast_tex->surf;

    // Compute and report texture signature
    uint32_t bound_width = src->desc.dwWidth;
    uint32_t bound_height = src->desc.dwHeight;

    md5_hasher mh;
    mh.add(make_span(&bound_width, 1).as_const_bytes());
    mh.add(make_span(&bound_height, 1).as_const_bytes());
    mh.add(make_span(src->buffer).as_const_bytes());

    auto sig = mh.finish();
    LOG_DEBUG("Loaded texture with signature ", static_cast<std::string>(sig));

    auto repl_map = surf->r->get_replacement_material(sig);

    if(repl_map.has_value()) {
        LOG_DEBUG("Found replacement");
        if((*repl_map)->albedo_map.has_value()) {
            surf->albedo_map = surf->r->get_srgb_texture_from_filename(*(*repl_map)->albedo_map);
        }

        if((*repl_map)->emissive_map.has_value()) {
            surf->emissive_map =
                surf->r->get_srgb_texture_from_filename(*(*repl_map)->emissive_map);
        }

        surf->albedo_factor = (*repl_map)->albedo_factor;
        surf->emissive_factor = (*repl_map)->emissive_factor;
        surf->alpha_mode = (*repl_map)->alpha_mode;
        surf->alpha_cutoff = (*repl_map)->alpha_cutoff;
        return D3D_OK;
    }

    // No replacements found. Create standard material.
    uint16_t const *in_em = (uint16_t const *)src->buffer.data();

    for(auto &out_em : src->conv_buffer) {
        // Convert from indexed to RGB888
        if(src->desc.ddpfPixelFormat.dwRGBAlphaBitMask) {
            // Convert from RGBA5551 to RGBA8888
            out_em = rgba5551_to_srgb_a8(*in_em);
            surf->alpha_mode = material_alpha_mode::blend;
        }
        else {
            // Convert from RGB565 to RGBA8888
            out_em = rgb565_to_srgb_a8(*in_em);
        }

        ++in_em;
    }

    surf->albedo_map = surf->r->create_srgb_texture_from_buffer(
        make_size((int)src->desc.dwWidth, (int)src->desc.dwHeight),
        make_span(src->conv_buffer).as_const_bytes());

    return D3D_OK;
}

jkgm::vidmem_texture_surface::vidmem_texture_surface(renderer *r, material_instance_id material_id)
    : DirectDrawSurface_impl("vidmem texture")
    , d3dtexture(this)
    , r(r)
    , material_id(material_id)
{
}

void jkgm::vidmem_texture_surface::clear()
{
    if(albedo_map.has_value()) {
        r->release_srgb_texture(*albedo_map);
        albedo_map.reset();
    }

    if(emissive_map.has_value()) {
        r->release_srgb_texture(*emissive_map);
        emissive_map.reset();
    }

    albedo_factor = color::fill(1.0f);
    emissive_factor = color_rgb::fill(0.0f);
    alpha_mode = material_alpha_mode::blend;
    alpha_cutoff = 0.5f;
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