#include "d3dtexture_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "error_reporter.hpp"

jkgm::Direct3DTexture_impl::Direct3DTexture_impl(char const *obj_name)
    : obj_name(obj_name)
{
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    report_unimplemented_function(
        str(format("Direct3DTexture(", obj_name, ")::QueryInterface(", to_string(riid), ")")));
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Initialize(LPDIRECT3DDEVICE a, LPDIRECTDRAWSURFACE b)
{
    report_unimplemented_function(str(format("Direct3DTexture(", obj_name, ")::Initialize")));
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::GetHandle(LPDIRECT3DDEVICE a, LPD3DTEXTUREHANDLE b)
{
    report_unimplemented_function(str(format("Direct3DTexture(", obj_name, ")::GetHandle")));
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::PaletteChanged(DWORD a, DWORD b)
{
    report_unimplemented_function(str(format("Direct3DTexture(", obj_name, ")::PaletteChanged")));
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Load(LPDIRECT3DTEXTURE a)
{
    report_unimplemented_function(str(format("Direct3DTexture(", obj_name, ")::Load")));
}

HRESULT WINAPI jkgm::Direct3DTexture_impl::Unload()
{
    report_unimplemented_function(str(format("Direct3DTexture(", obj_name, ")::Unload")));
}