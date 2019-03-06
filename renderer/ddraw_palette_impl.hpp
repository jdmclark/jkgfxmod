#pragma once

#include "renderer.hpp"
#include <ddraw.h>
#include <array>

namespace jkgm {
    class DirectDrawPalette_impl : public IDirectDrawPalette {
    private:
        renderer *r;

    public:
        std::array<PALETTEENTRY, 256> entries;

        explicit DirectDrawPalette_impl(renderer *r);

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetCaps(LPDWORD a) override;
        HRESULT WINAPI GetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d) override;
        HRESULT WINAPI Initialize(LPDIRECTDRAW a, DWORD b, LPPALETTEENTRY d) override;
        HRESULT WINAPI SetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d) override;
    };
}