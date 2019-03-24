#include "ddrawpalette_impl.hpp"
#include "base/log.hpp"
#include "dxguids.hpp"
#include "math/color_conv.hpp"
#include "renderer.hpp"

jkgm::DirectDrawPalette_impl::DirectDrawPalette_impl(renderer *r)
    : r(r)
{
    srgb_entries.resize(256, color_rgba8::zero());
    entries.resize(256);
}

void jkgm::DirectDrawPalette_impl::recompute_palette()
{
    for(size_t i = 0; i < 256; ++i) {
        auto src_col = entries[i];
        srgb_entries[i] = color_rgba8(src_col.peRed, src_col.peGreen, src_col.peBlue, uint8_t(255));
    }
}

HRESULT WINAPI jkgm::DirectDrawPalette_impl::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    LOG_ERROR("DirectDrawPalette::QueryInterface(", to_string(riid), ") unimplemented");
    abort();
}

ULONG WINAPI jkgm::DirectDrawPalette_impl::AddRef()
{
    // DirectDrawPalette is managed by the renderer. Refcount is intentionally not used.
    return 1000;
}

ULONG WINAPI jkgm::DirectDrawPalette_impl::Release()
{
    // DirectDrawPalette is managed by the renderer. Refcount is intentionally not used.
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
    std::copy(d, d + c, &entries[b]);
    recompute_palette();
    r->present_menu_surface_delayed();
    return DD_OK;
}