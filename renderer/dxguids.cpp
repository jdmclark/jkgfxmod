#include "dxguids.hpp"

char const* jkgm::to_string(REFIID riid)
{
    if(riid == IID_IDirectDraw) {
        return "IDirectDraw";
    }
    else if(riid == IID_IDirectDraw2) {
        return "IDirectDraw2";
    }
    else if(riid == IID_IDirectDraw4) {
        return "IDirectDraw4";
    }
    else if(riid == IID_IDirectDraw7) {
        return "IDirectDraw7";
    }
    else if(riid == IID_IDirectDrawSurface) {
        return "IDirectDrawSurface";
    }
    else if(riid == IID_IDirectDrawSurface2) {
        return "IDirectDrawSurface2";
    }
    else if(riid == IID_IDirectDrawSurface3) {
        return "IDirectDrawSurface3";
    }
    else if(riid == IID_IDirectDrawSurface4) {
        return "IDirectDrawSurface4";
    }
    else if(riid == IID_IDirectDrawSurface7) {
        return "IDirectDrawSurface7";
    }
    else if(riid == IID_IDirectDrawPalette) {
        return "IDirectDrawPalette";
    }
    else if(riid == IID_IDirectDrawClipper) {
        return "IDirectDrawClipper";
    }
    else if(riid == IID_IDirectDrawColorControl) {
        return "IDirectDrawColorControl";
    }
    else if(riid == IID_IDirectDrawGammaControl) {
        return "IDirectDrawGammaControl";
    }
    else if(riid == IID_IDirect3D) {
        return "IDirect3D";
    }
    else if(riid == IID_IDirect3D2) {
        return "IDirect3D2";
    }
    else if(riid == IID_IDirect3D3) {
        return "IDirect3D3";
    }
    else if(riid == IID_IDirect3D7) {
        return "IDirect3D7";
    }
    else if(riid == IID_IDirect3DRampDevice) {
        return "IDirect3DRampDevice";
    }
    else if(riid == IID_IDirect3DRGBDevice) {
        return "IDirect3DRGBDevice";
    }
    else if(riid == IID_IDirect3DHALDevice) {
        return "IDirect3DHALDevice";
    }
    else if(riid == IID_IDirect3DMMXDevice) {
        return "IDirect3DMMXDevice";
    }
    else if(riid == IID_IDirect3DRefDevice) {
        return "IDirect3DRefDevice";
    }
    else if(riid == IID_IDirect3DNullDevice) {
        return "IDirect3DNullDevice";
    }
    else if(riid == IID_IDirect3DTnLHalDevice) {
        return "IDirect3DTnLHalDevice";
    }
    else if(riid == IID_IDirect3DDevice) {
        return "IDirect3DDevice";
    }
    else if(riid == IID_IDirect3DDevice2) {
        return "IDirect3DDevice2";
    }
    else if(riid == IID_IDirect3DDevice3) {
        return "IDirect3DDevice3";
    }
    else if(riid == IID_IDirect3DDevice7) {
        return "IDirect3DDevice7";
    }
    else if(riid == IID_IDirect3DTexture) {
        return "IDirect3DTexture";
    }
    else if(riid == IID_IDirect3DTexture2) {
        return "IDirect3DTexture2";
    }
    else if(riid == IID_IDirect3DLight) {
        return "IDirect3DLight";
    }
    else if(riid == IID_IDirect3DMaterial) {
        return "IDirect3DMaterial";
    }
    else if(riid == IID_IDirect3DMaterial2) {
        return "IDirect3DMaterial2";
    }
    else if(riid == IID_IDirect3DMaterial3) {
        return "IDirect3DMaterial3";
    }
    else if(riid == IID_IDirect3DExecuteBuffer) {
        return "IDirect3DExecuteBuffer";
    }
    else if(riid == IID_IDirect3DViewport) {
        return "IDirect3DViewport";
    }
    else if(riid == IID_IDirect3DViewport2) {
        return "IDirect3DViewport2";
    }
    else if(riid == IID_IDirect3DViewport3) {
        return "IDirect3DViewport3";
    }
    else if(riid == IID_IDirect3DVertexBuffer) {
        return "IDirect3DVertexBuffer";
    }
    else if(riid == IID_IDirect3DVertexBuffer7) {
        return "IDirect3DVertexBuffer7";
    }
    else {
        return "Unknown";
    }
}