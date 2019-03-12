#pragma once

#include "math/color.hpp"
#include "renderer.hpp"
#include <array>
#include <ddraw.h>
#include <vector>

namespace jkgm {
    class DirectDrawPalette_impl : public IDirectDrawPalette {
    private:
        renderer *r;

    public:
        explicit DirectDrawPalette_impl(renderer *r);

        std::vector<color_rgba8> srgb_entries;
        std::vector<PALETTEENTRY> entries;

        void recompute_palette();

        HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;
        ULONG WINAPI AddRef() override;
        ULONG WINAPI Release() override;

        HRESULT WINAPI GetCaps(LPDWORD a) override;
        HRESULT WINAPI GetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d) override;
        HRESULT WINAPI Initialize(LPDIRECTDRAW a, DWORD b, LPPALETTEENTRY d) override;
        HRESULT WINAPI SetEntries(DWORD a, DWORD b, DWORD c, LPPALETTEENTRY d) override;
    };
}