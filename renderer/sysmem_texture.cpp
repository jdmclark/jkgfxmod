#include "sysmem_texture.hpp"
#include "base/log.hpp"
#include "common/error_reporter.hpp"
#include "dxguids.hpp"

jkgm::sysmem_texture::sysmem_texture(sysmem_texture_surface *surf)
    : Direct3DTexture_impl("sysmem")
    , surf(surf)
{
}

ULONG WINAPI jkgm::sysmem_texture::AddRef()
{
    return surf->AddRef();
}

ULONG WINAPI jkgm::sysmem_texture::Release()
{
    return surf->Release();
}

jkgm::sysmem_texture_surface::sysmem_texture_surface(size_t num_pixels)
    : DirectDrawSurface_impl("sysmem texture")
    , d3dtexture(this)
    , num_pixels(num_pixels)
{
    buffer.resize(num_pixels * 2);
    conv_buffer.resize(num_pixels, color_rgba8::zero());
}

void jkgm::sysmem_texture_surface::set_surface_desc(DDSURFACEDESC const &desc)
{
    // Basic sanity checking
    if(desc.lpSurface != nullptr) {
        report_unimplemented_function(
            "DirectDraw::CreateSurface(sysmem texture) with a creation-time buffer");
    }

    if(desc.ddpfPixelFormat.dwRGBBitCount != 16) {
        report_unimplemented_function(
            "DirectDraw::CreateSurface(sysmem texture) for a non-16 bit texture");
    }

    this->desc = desc;
    this->desc.lPitch = desc.dwWidth * (desc.ddpfPixelFormat.dwRGBBitCount / 8);
}

HRESULT WINAPI jkgm::sysmem_texture_surface::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == IID_IDirect3DTexture) {
        *ppvObj = &d3dtexture;
        AddRef();
        return S_OK;
    }

    return DirectDrawSurface_impl::QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::sysmem_texture_surface::AddRef()
{
    return ++refct;
}

ULONG WINAPI jkgm::sysmem_texture_surface::Release()
{
    return --refct;
}

HRESULT WINAPI jkgm::sysmem_texture_surface::GetSurfaceDesc(LPDDSURFACEDESC a)
{
    *a = desc;
    return DD_OK;
}

HRESULT WINAPI jkgm::sysmem_texture_surface::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    *b = desc;
    b->dwFlags = b->dwFlags | DDSD_LPSURFACE;
    b->lpSurface = buffer.data();
    return DD_OK;
}

HRESULT WINAPI jkgm::sysmem_texture_surface::Unlock(LPVOID a)
{
    return DD_OK;
}