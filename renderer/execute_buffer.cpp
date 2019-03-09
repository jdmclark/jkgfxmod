#include "execute_buffer.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::execute_buffer::execute_buffer(size_t bufsz)
    : bufsz(bufsz)
{
    buffer.resize(bufsz);
}

HRESULT WINAPI jkgm::execute_buffer::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("Direct3DExecuteBuffer::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented Direct3DExecuteBuffer::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::execute_buffer::AddRef()
{
    return ++refct;
}

ULONG WINAPI jkgm::execute_buffer::Release()
{
    return --refct;
}

HRESULT WINAPI jkgm::execute_buffer::Initialize(LPDIRECT3DDEVICE a, LPD3DEXECUTEBUFFERDESC b)
{
    LOG_ERROR("Direct3DExecuteBuffer::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::execute_buffer::Lock(LPD3DEXECUTEBUFFERDESC a)
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

HRESULT WINAPI jkgm::execute_buffer::Unlock()
{
    LOG_DEBUG("Direct3DExecuteBuffer::Unlock");
    return D3D_OK;
}

HRESULT WINAPI jkgm::execute_buffer::SetExecuteData(LPD3DEXECUTEDATA a)
{
    LOG_DEBUG("Direct3DExecuteBuffer::SetExecuteData");
    exec_data = *a;

    return D3D_OK;
}

HRESULT WINAPI jkgm::execute_buffer::GetExecuteData(LPD3DEXECUTEDATA a)
{
    *a = exec_data;
    return D3D_OK;
}

HRESULT WINAPI jkgm::execute_buffer::Validate(LPDWORD a, LPD3DVALIDATECALLBACK b, LPVOID c, DWORD d)
{
    LOG_ERROR("Direct3DExecuteBuffer::Validate unimplemented");
    abort();
}

HRESULT WINAPI jkgm::execute_buffer::Optimize(DWORD a)
{
    LOG_ERROR("Direct3DExecuteBuffer::Optimize unimplemented");
    abort();
}