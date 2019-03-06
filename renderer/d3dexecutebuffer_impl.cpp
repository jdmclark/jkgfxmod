#include "d3dexecutebuffer_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::Direct3DExecuteBuffer_impl::Direct3DExecuteBuffer_impl(renderer *r, size_t bufsz)
    : r(r)
    , bufsz(bufsz)
{
    buffer.resize(bufsz * 32);
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DExecuteBuffer::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3D::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::Direct3DExecuteBuffer_impl::AddRef()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DExecuteBuffer::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::Direct3DExecuteBuffer_impl::Release()
{
    // Direct3DDevice is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("Direct3DExecuteBuffer::Release");
    return 1000;
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::Initialize(LPDIRECT3DDEVICE a,
                                                            LPD3DEXECUTEBUFFERDESC b)
{
    LOG_ERROR("Direct3DExecuteBuffer::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::Lock(LPD3DEXECUTEBUFFERDESC a)
{
    LOG_DEBUG("Direct3DExecuteBuffer::Lock");

    D3DEXECUTEBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.dwSize = sizeof(desc);
    desc.dwFlags = D3DDEB_BUFSIZE | D3DDEB_LPDATA;
    desc.dwBufferSize = bufsz;
    desc.lpData = buffer.data();

    LOG_DEBUG("Desc size: ", sizeof(desc), ", target size: ", a->dwSize);
    CopyMemory(a, &desc, sizeof(desc));

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::Unlock()
{
    LOG_DEBUG("Direct3DExecuteBuffer::Unlock");

    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::SetExecuteData(LPD3DEXECUTEDATA a)
{
    LOG_DEBUG("Direct3DExecuteBuffer::SetExecuteData ignored");
    // HACK: Definitely needs to be implemented
    //exec_data = *a;
    return D3D_OK;
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::GetExecuteData(LPD3DEXECUTEDATA a)
{
    LOG_ERROR("Direct3DExecuteBuffer::GetExecuteData unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::Validate(LPDWORD a,
                                                          LPD3DVALIDATECALLBACK b,
                                                          LPVOID c,
                                                          DWORD d)
{
    LOG_ERROR("Direct3DExecuteBuffer::Validate unimplemented");
    abort();
}

HRESULT WINAPI jkgm::Direct3DExecuteBuffer_impl::Optimize(DWORD a)
{
    LOG_ERROR("Direct3DExecuteBuffer::Optimize unimplemented");
    abort();
}