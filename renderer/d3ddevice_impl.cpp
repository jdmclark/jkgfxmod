#include "d3ddevice_impl.hpp"
#include "base/log.hpp"
#include "common/error_reporter.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"

jkgm::Direct3DDevice_impl::Direct3DDevice_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    report_unimplemented_function(
        str(format("Direct3DDevice::QueryInterface(", to_string(riid), ")")));
}

ULONG WINAPI jkgm::Direct3DDevice_impl::AddRef()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::Direct3DDevice_impl::Release()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Initialize(LPDIRECT3D a, LPGUID b, LPD3DDEVICEDESC c)
{
    report_unimplemented_function("Direct3DDevice::Initialize");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetCaps(LPD3DDEVICEDESC, LPD3DDEVICEDESC)
{
    report_unimplemented_function("Direct3DDevice::GetCaps");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::SwapTextureHandles(LPDIRECT3DTEXTURE a,
                                                             LPDIRECT3DTEXTURE b)
{
    report_unimplemented_function("Direct3DDevice::SwapTextureHandles");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::CreateExecuteBuffer(LPD3DEXECUTEBUFFERDESC a,
                                                              LPDIRECT3DEXECUTEBUFFER *b,
                                                              IUnknown *c)
{
    if(a->lpData != NULL) {
        report_unimplemented_function(
            str(format("Direct3DDevice::CreateExecuteBuffer with creation time data")));
    }

    *b = r->create_direct3dexecutebuffer(a->dwBufferSize);
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetStats(LPD3DSTATS a)
{
    report_unimplemented_function("Direct3DDevice::GetStats");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Execute(LPDIRECT3DEXECUTEBUFFER a,
                                                  LPDIRECT3DVIEWPORT b,
                                                  DWORD c)
{
    r->execute_game(a, b);
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::AddViewport(LPDIRECT3DVIEWPORT a)
{
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::DeleteViewport(LPDIRECT3DVIEWPORT a)
{
    report_unimplemented_function("Direct3DDevice::DeleteViewport");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::NextViewport(LPDIRECT3DVIEWPORT a,
                                                       LPDIRECT3DVIEWPORT *b,
                                                       DWORD c)
{
    report_unimplemented_function("Direct3DDevice::NextViewport");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::Pick(LPDIRECT3DEXECUTEBUFFER a,
                                               LPDIRECT3DVIEWPORT b,
                                               DWORD c,
                                               LPD3DRECT d)
{
    report_unimplemented_function("Direct3DDevice::Pick");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetPickRecords(LPDWORD a, LPD3DPICKRECORD b)
{
    report_unimplemented_function("Direct3DDevice::GetPickRecords");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::EnumTextureFormats(LPD3DENUMTEXTUREFORMATSCALLBACK a,
                                                             LPVOID b)
{
    _DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_PIXELFORMAT | DDSD_CAPS;
    ddsd.dwWidth = 4096;
    ddsd.dwHeight = 4096;
    ddsd.dwMipMapCount = 0;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);

    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

    a(&ddsd, b);

    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

    a(&ddsd, b);

    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

    a(&ddsd, b);

    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x000007E0;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x0000F800;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

    a(&ddsd, b);

    ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x000003E0;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x00007C00;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00008000;

    a(&ddsd, b);

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::CreateMatrix(LPD3DMATRIXHANDLE a)
{
    report_unimplemented_function("Direct3DDevice::CreateMatrix");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::SetMatrix(D3DMATRIXHANDLE a, const LPD3DMATRIX b)
{
    report_unimplemented_function("Direct3DDevice::SetMatrix");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetMatrix(D3DMATRIXHANDLE a, LPD3DMATRIX b)
{
    report_unimplemented_function("Direct3DDevice::GetMatrix");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::DeleteMatrix(D3DMATRIXHANDLE a)
{
    report_unimplemented_function("Direct3DDevice::DeleteMatrix");
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::BeginScene()
{
    r->begin_game();
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::EndScene()
{
    r->end_game();
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DDevice_impl::GetDirect3D(LPDIRECT3D *a)
{
    report_unimplemented_function("Direct3DDevice::GetDirect3D");
}