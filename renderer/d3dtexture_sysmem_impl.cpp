#include "d3dtexture_sysmem_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DTexture_sysmem_impl::Direct3DTexture_sysmem_impl(renderer *r, DirectDraw_sysmem_texture_surface_impl *surf)
    : r(r)
    , surf(surf)
{
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DTexture(sysmem)::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3D::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::Direct3DTexture_sysmem_impl::AddRef()
{
    // Direct3DTexture_sysmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(sysmem)::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::Direct3DTexture_sysmem_impl::Release()
{
    // Direct3DTexture_sysmem_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture(sysmem)::Release");
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::Initialize(LPDIRECT3DDEVICE a, LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("Direct3DTexture(sysmem)::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    LOG_ERROR("Direct3DTexture(sysmem)::GetHandle");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::PaletteChanged(DWORD a, DWORD b)
{
    LOG_ERROR("Direct3DTexture(sysmem)::PaletteChanged unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::Load(LPDIRECT3DTEXTURE a)
{
    LOG_ERROR("Direct3DTexture(sysmem)::Load unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_sysmem_impl::Unload()
{
    LOG_ERROR("Direct3DTexture(sysmem)::Unload unimplemented");
    abort();
}