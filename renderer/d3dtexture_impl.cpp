#include "d3dtexture_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DTexture_impl::Direct3DTexture_impl(char const *obj_name)
    : obj_name(obj_name)
{
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR(
        "Direct3DTexture(", obj_name, ")::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Initialize(LPDIRECT3DDEVICE a, LPDIRECTDRAWSURFACE b)
{
    LOG_ERROR("Direct3DTexture(", obj_name, ")::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    LOG_ERROR("Direct3DTexture(", obj_name, ")::GetHandle");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::PaletteChanged(DWORD a, DWORD b)
{
    LOG_ERROR("Direct3DTexture(", obj_name, ")::PaletteChanged unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Load(LPDIRECT3DTEXTURE a)
{
    LOG_ERROR("Direct3DTexture(", obj_name, ")::Load unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Unload()
{
    LOG_ERROR("Direct3DTexture(", obj_name, ")::Unload unimplemented");
    abort();
}