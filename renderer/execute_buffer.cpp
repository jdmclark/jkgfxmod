#include "execute_buffer.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "error_reporter.hpp"

jkgm::execute_buffer::execute_buffer(size_t bufsz)
    : bufsz(bufsz)
{
    buffer.resize(bufsz);
}

HRESULT WINAPI jkgm::execute_buffer::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    report_unimplemented_function(
        str(format("Direct3DExecuteBuffer::QueryInterface(", to_string(riid), ")")));
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
    report_unimplemented_function("Direct3DExecuteBuffer::Initialize");
}

HRESULT WINAPI jkgm::execute_buffer::Lock(LPD3DEXECUTEBUFFERDESC a)
{
    D3DEXECUTEBUFFERDESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.dwSize = sizeof(desc);
    desc.dwFlags = D3DDEB_BUFSIZE | D3DDEB_LPDATA;
    desc.dwBufferSize = bufsz;
    desc.lpData = buffer.data();

    CopyMemory(a, &desc, sizeof(desc));

    return D3D_OK;
}

HRESULT WINAPI jkgm::execute_buffer::Unlock()
{
    return D3D_OK;
}

HRESULT WINAPI jkgm::execute_buffer::SetExecuteData(LPD3DEXECUTEDATA a)
{
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
    report_unimplemented_function("Direct3DExecuteBuffer::Validate");
}

HRESULT WINAPI jkgm::execute_buffer::Optimize(DWORD a)
{
    report_unimplemented_function("Direct3DExecuteBuffer::Optimize");
}