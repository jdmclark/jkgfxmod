#include "base/default_logger.hpp"
#include "base/log.hpp"
#include "base/span.hpp"
#include "ddraw_impl.hpp"
#include "renderer.hpp"
#include <Windows.h>
#include <detours/detours.h>

// DirectX hooks

using DirectDrawCreate_type = HRESULT(WINAPI *)(GUID *lpGUID,
                                                LPDIRECTDRAW *lplpDD,
                                                IUnknown *pUnkOuter);

static DirectDrawCreate_type TrueDirectDrawCreate = nullptr;

static std::vector<std::unique_ptr<jkgm::DirectDraw_impl>> ddraw_interfaces;
HRESULT WINAPI HookedDirectDrawCreate(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
{
    LOG_DEBUG("DirectDrawCreate hook called");

    LPDIRECTDRAW internal_interface;
    auto rv = TrueDirectDrawCreate(lpGUID, &internal_interface, pUnkOuter);

    ddraw_interfaces.push_back(std::make_unique<jkgm::DirectDraw_impl>(internal_interface));

    *lplpDD = ddraw_interfaces.back().get();

    return rv;
}

using DirectDrawEnumerateA_type = HRESULT(WINAPI *)(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext);

static DirectDrawEnumerateA_type TrueDirectDrawEnumerateA = nullptr;

HRESULT WINAPI HookedDirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext)
{
    LOG_DEBUG("DirectDrawEnumerateA hook called");
    return TrueDirectDrawEnumerateA(lpCallback, lpContext);
}

// Win32 API hooks

using CreateWindowExA_type = HWND(WINAPI *)(DWORD dwExStyle,
                                            LPCSTR lpClassName,
                                            LPCSTR lpWindowName,
                                            DWORD dwStyle,
                                            int X,
                                            int Y,
                                            int nWidth,
                                            int nHeight,
                                            HWND hWndParent,
                                            HMENU hMenu,
                                            HINSTANCE hInstance,
                                            LPVOID lpParam);

static CreateWindowExA_type TrueCreateWindowExA = nullptr;

static bool main_window_seen = false;
static std::unique_ptr<jkgm::renderer> the_renderer;

static HWND jk_window = NULL;
static std::string const jk_window_class_name = "wKernel";

HWND WINAPI HookedCreateWindowExA(DWORD dwExStyle,
                                  LPCSTR lpClassName,
                                  LPCSTR lpWindowName,
                                  DWORD dwStyle,
                                  int X,
                                  int Y,
                                  int nWidth,
                                  int nHeight,
                                  HWND hWndParent,
                                  HMENU hMenu,
                                  HINSTANCE hInstance,
                                  LPVOID lpParam)
{
    if(!main_window_seen) {
        LOG_DEBUG("CreateWindowExA hook called");

        auto rv = TrueCreateWindowExA(dwExStyle,
                                      lpClassName,
                                      lpWindowName,
                                      dwStyle,
                                      X,
                                      Y,
                                      nWidth,
                                      nHeight,
                                      hWndParent,
                                      hMenu,
                                      hInstance,
                                      lpParam);
        if(jk_window_class_name == lpClassName) {
            LOG_DEBUG("Detected main window creation");
            main_window_seen = true;
            the_renderer->initialize();
        }

        return rv;
    }
    else {
        return TrueCreateWindowExA(dwExStyle,
                                   lpClassName,
                                   lpWindowName,
                                   dwStyle,
                                   X,
                                   Y,
                                   nWidth,
                                   nHeight,
                                   hWndParent,
                                   hMenu,
                                   hInstance,
                                   lpParam);
    }
}

using GetDC_type = HDC(WINAPI *)(HWND hWnd);
static GetDC_type TrueGetDC = nullptr;
HDC WINAPI HookedGetDC(HWND hWnd)
{
    LOG_DEBUG("GetDC hook called");
    return TrueGetDC(hWnd);
}

using ReleaseDC_type = int(WINAPI *)(HWND hWnd, HDC hDC);
static ReleaseDC_type TrueReleaseDC = nullptr;
int WINAPI HookedReleaseDC(HWND hWnd, HDC hDC)
{
    LOG_DEBUG("ReleaseDC hook called");
    return TrueReleaseDC(hWnd, hDC);
}

using BeginPaint_type = HDC(WINAPI *)(HWND hWnd, LPPAINTSTRUCT lpPaint);
static BeginPaint_type TrueBeginPaint = nullptr;
HDC WINAPI HookedBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    LOG_DEBUG("BeginPaint hook called");
    return TrueBeginPaint(hWnd, lpPaint);
}

using EndPaint_type = BOOL(WINAPI *)(HWND hWnd, PAINTSTRUCT CONST *lpPaint);
static EndPaint_type TrueEndPaint = nullptr;
BOOL WINAPI HookedEndPaint(HWND hWnd, PAINTSTRUCT CONST *lpPaint)
{
    LOG_DEBUG("EndPaint hook called");
    return TrueEndPaint(hWnd, lpPaint);
}

using CreateDIBSection_type = HBITMAP(WINAPI *)(HDC hDC,
                                                BITMAPINFO const *pbmi,
                                                UINT usage,
                                                VOID **ppvBits,
                                                HANDLE hSection,
                                                DWORD offset);
static CreateDIBSection_type TrueCreateDIBSection = nullptr;
HBITMAP WINAPI HookedCreateDIBSection(HDC hDC,
                                      BITMAPINFO const *pbmi,
                                      UINT usage,
                                      VOID **ppvBits,
                                      HANDLE hSection,
                                      DWORD offset)
{
    LOG_DEBUG("CreateDIBSection hook called");
    auto rv = TrueCreateDIBSection(hDC, pbmi, usage, ppvBits, hSection, offset);
    the_renderer->set_menu_source(reinterpret_cast<char const *>(*ppvBits));
    return rv;
}

using SetDIBColorTable_type = UINT(WINAPI *)(HDC hDC,
                                             UINT iStart,
                                             UINT cEntries,
                                             RGBQUAD const *prgbq);
static SetDIBColorTable_type TrueSetDIBColorTable = nullptr;
UINT WINAPI HookedSetDIBColorTable(HDC hDC, UINT iStart, UINT cEntries, RGBQUAD const *prgbq)
{
    LOG_DEBUG("SetDIBColorTable hook called");
    the_renderer->set_menu_palette(iStart, jkgm::make_span(prgbq, cEntries));
    return TrueSetDIBColorTable(hDC, iStart, cEntries, prgbq);
}

using GdiFlush_type = BOOL(WINAPI *)();
static GdiFlush_type TrueGdiFlush = nullptr;
BOOL WINAPI HookedGdiFlush()
{
    LOG_DEBUG("GdiFlush hook called");
    the_renderer->present_menu();
    return TrueGdiFlush();
}

__declspec(dllexport) BOOL WINAPI export_workaround()
{
    // Detours seems to require at least one export in order to properly link the DLL
    return TRUE;
}

bool attach_hooks()
{
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Injected functions

    // DirectX
    TrueDirectDrawCreate =
        (DirectDrawCreate_type)DetourFindFunction("ddraw.dll", "DirectDrawCreate");
    DetourAttach(&(PVOID &)TrueDirectDrawCreate, HookedDirectDrawCreate);

    TrueDirectDrawEnumerateA =
        (DirectDrawEnumerateA_type)DetourFindFunction("ddraw.dll", "DirectDrawEnumerateA");
    DetourAttach(&(PVOID &)TrueDirectDrawEnumerateA, HookedDirectDrawEnumerateA);

    // Win32 API
    TrueCreateWindowExA = (CreateWindowExA_type)DetourFindFunction("user32.dll", "CreateWindowExA");
    DetourAttach(&(PVOID &)TrueCreateWindowExA, HookedCreateWindowExA);

    TrueGetDC = (GetDC_type)DetourFindFunction("user32.dll", "GetDC");
    DetourAttach(&(PVOID &)TrueGetDC, HookedGetDC);

    TrueReleaseDC = (ReleaseDC_type)DetourFindFunction("user32.dll", "ReleaseDC");
    DetourAttach(&(PVOID &)TrueReleaseDC, HookedReleaseDC);

    TrueBeginPaint = (BeginPaint_type)DetourFindFunction("user32.dll", "BeginPaint");
    DetourAttach(&(PVOID &)TrueBeginPaint, HookedBeginPaint);

    TrueEndPaint = (EndPaint_type)DetourFindFunction("user32.dll", "EndPaint");
    DetourAttach(&(PVOID &)TrueEndPaint, HookedEndPaint);

    TrueCreateDIBSection =
        (CreateDIBSection_type)DetourFindFunction("gdi32.dll", "CreateDIBSection");
    DetourAttach(&(PVOID &)TrueCreateDIBSection, HookedCreateDIBSection);

    TrueSetDIBColorTable =
        (SetDIBColorTable_type)DetourFindFunction("gdi32.dll", "SetDIBColorTable");
    DetourAttach(&(PVOID &)TrueSetDIBColorTable, HookedSetDIBColorTable);

    TrueGdiFlush = (GdiFlush_type)DetourFindFunction("gdi32.dll", "GdiFlush");
    DetourAttach(&(PVOID &)TrueGdiFlush, HookedGdiFlush);

    LONG error = DetourTransactionCommit();
    return error == NO_ERROR;
}

bool detach_hooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Detach injected functions
    DetourDetach(&(PVOID &)TrueDirectDrawCreate, HookedDirectDrawCreate);
    DetourDetach(&(PVOID &)TrueDirectDrawEnumerateA, HookedDirectDrawEnumerateA);

    LONG error = DetourTransactionCommit();
    return error == NO_ERROR;
}

constexpr wchar_t const *appname = L"JkGfxMod";
void show_error_message(wchar_t const *message)
{
    MessageBoxW(NULL, message, appname, MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    if(DetourIsHelperProcess()) {
        return TRUE;
    }

    if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
        jkgm::setup_default_logging();

        if(!the_renderer) {
            the_renderer = jkgm::create_renderer(hModule);
        }

        LOG_DEBUG("Attaching renderer to process");

        if(!attach_hooks()) {
            LOG_ERROR("Failed to commit Detours hook transaction");
            show_error_message(L"Failed to install renderer");
            return FALSE;
        }

        LOG_DEBUG("Finished attaching renderer to process");
    }
    else if(ul_reason_for_call == DLL_PROCESS_DETACH) {
        detach_hooks();
    }

    return TRUE;
}
