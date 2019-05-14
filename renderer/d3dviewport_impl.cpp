#include "d3dviewport_impl.hpp"
#include "base/log.hpp"
#include "common/error_reporter.hpp"
#include "dxguids.hpp"

jkgm::Direct3DViewport_impl::Direct3DViewport_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    report_unimplemented_function(
        str(format("Direct3DViewport::QueryInterface(", to_string(riid), ")")));
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
    report_unimplemented_function("Direct3DViewport::Initialize");
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
    report_unimplemented_function("Direct3DViewport::TransformVertices");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::LightElements(DWORD a, LPD3DLIGHTDATA b)
{
    report_unimplemented_function("Direct3DViewport::LightElements");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::SetBackground(D3DMATERIALHANDLE a)
{
    report_unimplemented_function("Direct3DViewport::SetBackground");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::GetBackground(LPD3DMATERIALHANDLE a, LPBOOL b)
{
    report_unimplemented_function("Direct3DViewport::GetBackground");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::SetBackgroundDepth(LPDIRECTDRAWSURFACE a)
{
    report_unimplemented_function("Direct3DViewport::SetBackgroundDepth");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::GetBackgroundDepth(LPDIRECTDRAWSURFACE *a, LPBOOL b)
{
    report_unimplemented_function("Direct3DViewport::GetBackgroundDepth");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::Clear(DWORD a, LPD3DRECT b, DWORD c)
{
    report_unimplemented_function("Direct3DViewport::Clear");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::AddLight(LPDIRECT3DLIGHT a)
{
    report_unimplemented_function("Direct3DViewport::AddLight");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::DeleteLight(LPDIRECT3DLIGHT a)
{
    report_unimplemented_function("Direct3DViewport::DeleteLight");
}

HRESULT WINAPI jkgm::Direct3DViewport_impl::NextLight(LPDIRECT3DLIGHT a,
                                                      LPDIRECT3DLIGHT *b,
                                                      DWORD c)
{
    report_unimplemented_function("Direct3DViewport::NextLight");
}