#include "ddraw_palette_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"

jkgm::DirectDrawPalette_impl::DirectDrawPalette_impl(renderer *r)
    : r(r)
{
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_DEBUG("DirectDrawPalette::QueryInterface(", to_string(riid), ")");
    LOG_ERROR("Called unimplemented DirectDrawPalette::QueryInterface");
    abort();
}

ULONG WINAPI jkgm::DirectDrawPalette_impl::AddRef()
{
    // DirectDrawPalette is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawPalette::AddRef");
    return 1000;
}

ULONG WINAPI jkgm::DirectDrawPalette_impl::Release()
{
    // DirectDrawPalette is managed by the renderer. Refcount is intentionally not used.
    LOG_DEBUG("DirectDrawPalette::Release");
    return 1000;
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::GetCaps(LPDWORD a)
{
    LOG_ERROR("DirectDrawPalette::GetCaps unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::GetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d)
{
    LOG_ERROR("DirectDrawPalette::GetEntries unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::Initialize(LPDIRECTDRAW a, DWORD b, LPPALETTEENTRY d)
{
    LOG_ERROR("DirectDrawPalette::Initialize unimplemented");
    abort();
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::SetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d)
{
    LOG_ERROR("DirectDrawPalette::SetEntries unimplemented");
    abort();
}