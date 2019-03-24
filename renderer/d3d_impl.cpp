#include "d3d_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "renderer.hpp"

jkgm::Direct3D_impl::Direct3D_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::Direct3D_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR("Direct3D::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

ULONG WINAPI jkgm::Direct3D_impl::AddRef()
{
    // Direct3D is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::Direct3D_impl::Release()
{
    // Direct3D is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

HRESULT WINAPI jkgm::Direct3D_impl::Initialize(REFCLSID a)
{
    LOG_ERROR("Direct3D::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3D_impl::EnumDevices(LPD3DENUMDEVICESCALLBACK a, LPVOID b)
{
    // Construct a single virtual device, supporting HAL but not software
    _D3DDeviceDesc hal_dev;
    ZeroMemory(&hal_dev, sizeof(hal_dev));

    hal_dev.dwSize = sizeof(hal_dev);
    hal_dev.dwFlags = D3DDD_BCLIPPING | D3DDD_COLORMODEL | D3DDD_DEVCAPS |
                      D3DDD_DEVICERENDERBITDEPTH | D3DDD_DEVICEZBUFFERBITDEPTH |
                      D3DDD_LIGHTINGCAPS | D3DDD_LINECAPS | D3DDD_MAXBUFFERSIZE |
                      D3DDD_MAXVERTEXCOUNT | D3DDD_TRANSFORMCAPS | D3DDD_TRICAPS;
    hal_dev.dcmColorModel = D3DCOLOR_RGB;
    hal_dev.dwDevCaps = D3DDEVCAPS_FLOATTLVERTEX | D3DDEVCAPS_EXECUTESYSTEMMEMORY |
                        D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
                        D3DDEVCAPS_TLVERTEXVIDEOMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY |
                        D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_CANRENDERAFTERFLIP |
                        D3DDEVCAPS_DRAWPRIMITIVES2 | D3DDEVCAPS_DRAWPRIMITIVES2EX |
                        D3DDEVCAPS_HWTRANSFORMANDLIGHT;
    hal_dev.dtcTransformCaps.dwSize = sizeof(hal_dev.dtcTransformCaps);
    hal_dev.dtcTransformCaps.dwCaps = D3DTRANSFORMCAPS_CLIP;
    hal_dev.bClipping = TRUE;
    hal_dev.dlcLightingCaps.dwSize = sizeof(hal_dev.dlcLightingCaps);
    hal_dev.dlcLightingCaps.dwCaps = D3DLIGHTCAPS_POINT;
    hal_dev.dlcLightingCaps.dwLightingModel = D3DLIGHTINGMODEL_RGB | D3DLIGHTINGMODEL_MONO;
    hal_dev.dlcLightingCaps.dwNumLights = 256;

    hal_dev.dpcLineCaps.dwSize = sizeof(hal_dev.dpcLineCaps);
    hal_dev.dpcLineCaps.dwMiscCaps =
        D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_CULLNONE;
    hal_dev.dpcLineCaps.dwRasterCaps = D3DPRASTERCAPS_ZTEST;
    hal_dev.dpcLineCaps.dwZCmpCaps = 0x0FF;
    hal_dev.dpcLineCaps.dwSrcBlendCaps = 0x01FFF;
    hal_dev.dpcLineCaps.dwDestBlendCaps = 0x01FFF;
    hal_dev.dpcLineCaps.dwAlphaCmpCaps = 0x0FF;
    hal_dev.dpcLineCaps.dwShadeCaps = 0x001FFFFFL;
    hal_dev.dpcLineCaps.dwTextureCaps =
        D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_POW2 | D3DPTEXTURECAPS_ALPHA;
    hal_dev.dpcLineCaps.dwTextureFilterCaps = 0x03F;
    hal_dev.dpcLineCaps.dwTextureBlendCaps = 0x0FF;
    hal_dev.dpcLineCaps.dwTextureAddressCaps =
        D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_CLAMP;
    hal_dev.dpcLineCaps.dwStippleWidth = 0;
    hal_dev.dpcLineCaps.dwStippleHeight = 0;

    hal_dev.dpcTriCaps.dwSize = sizeof(hal_dev.dpcLineCaps);
    hal_dev.dpcTriCaps.dwMiscCaps =
        D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_CULLNONE;
    hal_dev.dpcTriCaps.dwRasterCaps = D3DPRASTERCAPS_ZTEST;
    hal_dev.dpcTriCaps.dwZCmpCaps = 0x0FF;
    hal_dev.dpcTriCaps.dwSrcBlendCaps = 0x01FFF;
    hal_dev.dpcTriCaps.dwDestBlendCaps = 0x01FFF;
    hal_dev.dpcTriCaps.dwAlphaCmpCaps = 0x0FF;
    hal_dev.dpcTriCaps.dwShadeCaps = 0x001FFFFFL;
    hal_dev.dpcTriCaps.dwTextureCaps =
        D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_POW2 | D3DPTEXTURECAPS_ALPHA;
    hal_dev.dpcTriCaps.dwTextureFilterCaps = 0x03F;
    hal_dev.dpcTriCaps.dwTextureBlendCaps = 0x0FF;
    hal_dev.dpcTriCaps.dwTextureAddressCaps =
        D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_CLAMP;
    hal_dev.dpcTriCaps.dwStippleWidth = 0;
    hal_dev.dpcTriCaps.dwStippleHeight = 0;

    hal_dev.dwDeviceRenderBitDepth = DDBD_16;
    hal_dev.dwDeviceZBufferBitDepth = DDBD_16;
    hal_dev.dwMaxBufferSize = 8388608;
    hal_dev.dwMaxVertexCount = 8388608;
    hal_dev.dwMinTextureWidth = 1;
    hal_dev.dwMinTextureHeight = 1;
    hal_dev.dwMaxTextureWidth = 4096;
    hal_dev.dwMaxTextureHeight = 4096;

    _D3DDeviceDesc hel_dev;
    CopyMemory(&hel_dev, &hal_dev, sizeof(hel_dev));

    std::string desc_buf = "JkGfxMod OpenGL Renderer";
    std::string dev_buf = "device";

    auto devguid = ReusablePrimaryDirect3DDevice;

    a(&devguid, desc_buf.data(), dev_buf.data(), &hal_dev, &hel_dev, b);

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3D_impl::CreateLight(LPDIRECT3DLIGHT *a, IUnknown *b)
{
    LOG_ERROR("Direct3D::CreateLight unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3D_impl::CreateMaterial(LPDIRECT3DMATERIAL *a, IUnknown *b)
{
    LOG_ERROR("Direct3D::CreateMaterial unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3D_impl::CreateViewport(LPDIRECT3DVIEWPORT *a, IUnknown *b)
{
    *a = r->get_direct3dviewport();
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3D_impl::FindDevice(LPD3DFINDDEVICESEARCH a, LPD3DFINDDEVICERESULT b)
{
    LOG_ERROR("Direct3D::FindDevice unimplemented");
    abort();
}