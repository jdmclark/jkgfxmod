#include "ddraw_vidmem_texture_surface.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDraw_vidmem_texture_surface_impl::DirectDraw_vidmem_texture_surface_impl(
    renderer *r,
    DDSURFACEDESC desc,
    size_t texture_index)
    : r(r)
    , d3dtexture(r, this)
    , desc(desc)
    , texture_index(texture_index)
{
    // Jedi Knight apparently generates 32-bit RGBA textures!?

    if(desc.lpSurface != nullptr) {
        LOG_ERROR("JK IS PASSING A BUFFER TO A VIDMEM TEXTURE");
        abort();
    }

    // Construct some pessimistic backbuffer size
    buffer.resize(desc.dwWidth * desc.dwHeight, solid(color_rgb8::zero()));
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::QueryInterface(REFIID riid,
                                                                            LPVOID *ppvObj)
{
    LOG_DEBUG("DirectDraw vidmem texture surface::QueryInterface(", to_string(riid), ")");

    if(riid == IID_IDirect3DTexture) {
        *ppvObj = &d3dtexture;
        return S_OK;
    }

    LOG_ERROR("Called unimplemented DirectDraw vidmem texture surface::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw vidmem texture surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw vidmem texture surface::Release");
    return 1000;
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::AddAttachedSurface(LPDIRECTDRAWSURFACE a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::AddAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::AddOverlayDirtyRect(LPRECT a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::AddOverlayDirtyRect unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Blt(LPRECT a,
                                                                 LPDIRECTDRAWSURFACE b,
                                                                 LPRECT c,
                                                                 DWORD d,
                                                                 LPDDBLTFX e)
{
    LOG_ERROR("DirectDraw vidmem texture surface::Blt unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::BltBatch(LPDDBLTBATCH a,
                                                                      DWORD b,
                                                                      DWORD c)
{
    LOG_ERROR("DirectDraw vidmem texture surface::BltBatch unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::BltFast(DWORD a,
                                                                     DWORD b,
                                                                     LPDIRECTDRAWSURFACE c,
                                                                     LPRECT d,
                                                                     DWORD e)
{
    LOG_ERROR("DirectDraw vidmem texture surface::BltFast unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::DeleteAttachedSurface(DWORD a,
                                                                        LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::DeleteAttachedSurface unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::EnumAttachedSurfaces(LPVOID a,
                                                                       LPDDENUMSURFACESCALLBACK b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::EnumAttachedSurfaces unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::EnumOverlayZOrders(DWORD a,
                                                                     LPVOID b,
                                                                     LPDDENUMSURFACESCALLBACK c)
{
    LOG_ERROR("DirectDraw vidmem texture surface::EnumOverlayZOrders unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::Flip unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::GetAttachedSurface(LPDDSCAPS a,
                                                                     LPDIRECTDRAWSURFACE *b)
{
    LOG_ERROR(
        "DirectDraw vidmem texture surface::GetAttachedSurface(", a->dwCaps, ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetBltStatus(DWORD a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetBltStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetCaps(LPDDSCAPS a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetClipper(LPDIRECTDRAWCLIPPER *a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetDC(HDC *a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetFlipStatus(DWORD a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetFlipStatus unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetOverlayPosition(LPLONG a, LPLONG b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetPalette(LPDIRECTDRAWPALETTE *a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetPixelFormat(LPDDPIXELFORMAT a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::GetPixelFormat unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    LOG_DEBUG("DirectDraw vidmem texture surface::GetSurfaceDesc(", a->dwFlags, ")");
    *a = desc;
    return DD_OK;
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Initialize(LPDIRECTDRAW a,
                                                                        LPDDSURFACEDESC b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::IsLost()
{
    LOG_ERROR("DirectDraw vidmem texture surface::IsLost unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Lock(LPRECT a,
                                                                  LPDDSURFACEDESC b,
                                                                  DWORD c,
                                                                  HANDLE d)
{
    LOG_ERROR("DirectDraw vidmem texture surface::Lock unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::ReleaseDC(HDC a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::ReleaseDC unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Restore()
{
    LOG_ERROR("DirectDraw vidmem texture surface::Restore unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::SetClipper(LPDIRECTDRAWCLIPPER a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::SetClipper unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::SetColorKey unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::SetOverlayPosition(LONG a, LONG b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::SetOverlayPosition unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::SetPalette unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::Unlock(LPVOID a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::Unlock");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::UpdateOverlay(LPRECT a,
                                                                           LPDIRECTDRAWSURFACE b,
                                                                           LPRECT c,
                                                                           DWORD d,
                                                                           LPDDOVERLAYFX e)
{
    LOG_ERROR("DirectDraw vidmem texture surface::UpdateOverlay unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDraw_vidmem_texture_surface_impl::UpdateOverlayDisplay(DWORD a)
{
    LOG_ERROR("DirectDraw vidmem texture surface::UpdateOverlayDisplay unimplemented");
    abort();
}

HRESULT WINAPI
    jkgm::DirectDraw_vidmem_texture_surface_impl::UpdateOverlayZOrder(DWORD a,
                                                                      LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("DirectDraw vidmem texture surface::UpdateOverlayZOrder unimplemented");
    abort();
}

void jkgm::DirectDraw_vidmem_texture_surface_impl::regenerate_texture()
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