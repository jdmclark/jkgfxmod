#include "d3ddevice_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DDevice_impl::Direct3DDevice_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DDevice::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3D::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::Direct3DDevice_impl::AddRef()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DDevice::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::Direct3DDevice_impl::Release()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DDevice::Release");
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Initialize(LPDIRECT3D a, LPGUID b, LPD3DDEVICEDESC c) {
    LOG_ERROR("Direct3DDevice::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetCaps(LPD3DDEVICEDESC, LPD3DDEVICEDESC)
{
    LOG_ERROR("Direct3DDevice::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::SwapTextureHandles(LPDIRECT3DTEXTURE a,
                                                             LPDIRECT3DTEXTURE b)
{
    LOG_ERROR("Direct3DDevice::SwapTextureHandles unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::CreateExecuteBuffer(LPD3DEXECUTEBUFFERDESC a,
                                                              LPDIRECT3DEXECUTEBUFFER *b,
                                                              IUnknown *c)
{
    LOG_ERROR("Direct3DDevice::CreateExecuteBuffer unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetStats(LPD3DSTATS a)
{
    LOG_ERROR("Direct3DDevice::GetStats unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Execute(LPDIRECT3DEXECUTEBUFFER a,
                                                  LPDIRECT3DVIEWPORT b,
                                                  DWORD c)
{
    LOG_ERROR("Direct3DDevice::Execute unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::AddViewport(LPDIRECT3DVIEWPORT a)
{
    LOG_ERROR("Direct3DDevice::AddViewport unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::DeleteViewport(LPDIRECT3DVIEWPORT a)
{
    LOG_ERROR("Direct3DDevice::DeleteViewport unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::NextViewport(LPDIRECT3DVIEWPORT a,
                                                       LPDIRECT3DVIEWPORT *b,
                                                       DWORD c)
{
    LOG_ERROR("Direct3DDevice::NextViewport unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Pick(LPDIRECT3DEXECUTEBUFFER a,
                                               LPDIRECT3DVIEWPORT b,
                                               DWORD c,
                                               LPD3DRECT d)
{
    LOG_ERROR("Direct3DDevice::Pick unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetPickRecords(LPDWORD a, LPD3DPICKRECORD b)
{
    LOG_ERROR("Direct3DDevice::GetPickRecords unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::EnumTextureFormats(LPD3DENUMTEXTUREFORMATSCALLBACK a,
                                                             LPVOID b)
{
    LOG_DEBUG("Direct3DDevice::EnumTextureFormats");

    _DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_PIXELFORMAT | DDSD_CAPS;
    ddsd.dwWidth = 4096;
    ddsd.dwHeight = 4096;
    ddsd.dwMipMapCount = 32;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
    ddsd.ddpfPixelFormat.dwAlphaBitDepth = 8;
    ddsd.ddpfPixelFormat.dwRBitMask = 0xFF;
    ddsd.ddpfPixelFormat.dwGBitMask = 0xFF00;
    ddsd.ddpfPixelFormat.dwBBitMask = 0xFF0000;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

    a(&ddsd, b);

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::CreateMatrix(LPD3DMATRIXHANDLE a)
{
    LOG_ERROR("Direct3DDevice::CreateMatrix unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::SetMatrix(D3DMATRIXHANDLE a, const LPD3DMATRIX b)
{
    LOG_ERROR("Direct3DDevice::SetMatrix unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetMatrix(D3DMATRIXHANDLE a, LPD3DMATRIX b)
{
    LOG_ERROR("Direct3DDevice::GetMatrix unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::DeleteMatrix(D3DMATRIXHANDLE a)
{
    LOG_ERROR("Direct3DDevice::DeleteMatrix unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::BeginScene()
{
    LOG_ERROR("Direct3DDevice::BeginScene unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::EndScene()
{
    LOG_ERROR("Direct3DDevice::EndScene unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetDirect3D(LPDIRECT3D *a)
{
    LOG_ERROR("Direct3DDevice::GetDirect3D unimplemented");
    abort();
}