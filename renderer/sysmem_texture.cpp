#include "sysmem_texture.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::sysmem_texture::sysmem_texture(renderer *r, sysmem_texture_surface *surf)
    : Direct3DTexture_impl("sysmem")
    , r(r)
    , surf(surf)
{
}

ULONG WINAPI jkgm::sysmem_texture::AddRef()
{
    // Direct3DTexture_sysmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(sysmem)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::sysmem_texture::Release()
{
    // Direct3DTexture_sysmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(sysmem)::Release");
    return 1000;
}

jkgm::sysmem_texture_surface::sysmem_texture_surface(renderer *r, DDSURFACEDESC desc)
    : DirectDrawSurface_impl("sysmem texture")
    , r(r)
    , d3dtexture(r, this)
    , desc(desc)
{
    if(desc.lpSurface != nullptr) {
        LOG_ERROR("JK IS PASSING A BUFFER TO A SYSMEM TEXTURE");
        abort();
    }

    LOG_DEBUG("Texture depth: ",
              desc.ddpfPixelFormat.dwRGBBitCount,
              " width: ",
              desc.dwWidth,
              " height: ",
              desc.dwHeight,
              " amask: ",
              desc.ddpfPixelFormat.dwRGBAlphaBitMask);

    // Construct some pessimistic backbuffer size
    this->desc.lPitch = desc.dwWidth * (desc.ddpfPixelFormat.dwRGBBitCount / 8);
    buffer.resize(desc.dwWidth * desc.dwHeight * (desc.ddpfPixelFormat.dwRGBBitCount / 8),
                  char(0x7C));
}

HRESULT WINAPI jkgm::sysmem_texture_surface::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if(riid == IID_IDirect3DTexture) {
        *ppvObj = &d3dtexture;
        return S_OK;
    }

    return DirectDrawSurface_impl::QueryInterface(riid, ppvObj);
}

ULONG WINAPI jkgm::sysmem_texture_surface::AddRef()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw sysmem texture surface::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::sysmem_texture_surface::Release()
{
    // Primary surface is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDraw sysmem texture surface::Release");
    return 1000;
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