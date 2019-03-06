#include "d3dtexture_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DTexture_impl::Direct3DTexture_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DTexture::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3D::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::Direct3DTexture_impl::AddRef()
{
    // Direct3DTexture_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::Direct3DTexture_impl::Release()
{
    // Direct3DTexture_impl is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DTexture::Release");
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Initialize(LPDIRECT3DDEVICE a, LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("Direct3DTexture::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    LOG_DEBUG("Direct3DTexture::GetHandle ignored");
    // HACK: This will definitely need to be implemented
    *b = 0x7C7C7C7CUL;
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::PaletteChanged(DWORD a, DWORD b)
{
    LOG_ERROR("Direct3DTexture::PaletteChanged unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Load(LPDIRECT3DTEXTURE a)
{
    LOG_DEBUG("Direct3DTexture::Load ignored");
    // HACK: This may need to be implemented
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Unload()
{
    LOG_ERROR("Direct3DTexture::Unload unimplemented");
    abort();
}