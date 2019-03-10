#include "d3dviewport_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DViewport_impl::Direct3DViewport_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR("Direct3DViewport::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

ULONG WINAPI jkgm::Direct3DViewport_impl::AddRef()
{
    // Direct3DViewport is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::Direct3DViewport_impl::Release()
{
    // Direct3DViewport is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::Initialize(LPDIRECT3D a)
{
    LOG_ERROR("Direct3DViewport::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::GetViewport(LPD3DVIEWPORT a)
{
    *a = data;
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::SetViewport(LPD3DVIEWPORT a)
{
    data = *a;
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::TransformVertices(DWORD a,
                                                              LPD3DTRANSFORMDATA b,
                                                              DWORD c,
                                                              LPDWORD d)
{
    LOG_ERROR("Direct3DViewport::TransformVertices unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::LightElements(DWORD a, LPD3DLIGHTDATA b)
{
    LOG_ERROR("Direct3DViewport::LightElements unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::SetBackground(D3DMATERIALHANDLE a)
{
    LOG_ERROR("Direct3DViewport::SetBackground unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::GetBackground(LPD3DMATERIALHANDLE a, LPBOOL b)
{
    LOG_ERROR("Direct3DViewport::GetBackground unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::SetBackgroundDepth(LPDIRECTDRAWSURFACE a)
{
    LOG_ERROR("Direct3DViewport::SetBackgroundDepth unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::GetBackgroundDepth(LPDIRECTDRAWSURFACE *a, LPBOOL b)
{
    LOG_ERROR("Direct3DViewport::GetBackgroundDepth unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::Clear(DWORD a, LPD3DRECT b, DWORD c)
{
    LOG_ERROR("Direct3DViewport::Clear unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::AddLight(LPDIRECT3DLIGHT a)
{
    LOG_ERROR("Direct3DViewport::AddLight unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::DeleteLight(LPDIRECT3DLIGHT a)
{
    LOG_ERROR("Direct3DViewport::DeleteLight unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::NextLight(LPDIRECT3DLIGHT a, LPDIRECT3DLIGHT *b, DWORD c)
{
    LOG_ERROR("Direct3DViewport::NextLight unimplemented");
    abort();
}