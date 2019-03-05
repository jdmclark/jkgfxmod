#include "base/default_logger.hpp"
#include "base/log.hpp"
#include "ddraw_impl.hpp"
#include <Windows.h>
#include <detours/detours.h>

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

constexpr wchar_t const *appname = L"JkGfxMod";
void show_error_message(wchar_t const *message)
{
    MessageBoxW(NULL, message, appname, MB_OK);
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
    TrueDirectDrawCreate =
        (DirectDrawCreate_type)DetourFindFunction("ddraw.dll", "DirectDrawCreate");
    DetourAttach(&(PVOID &)TrueDirectDrawCreate, HookedDirectDrawCreate);

    TrueDirectDrawEnumerateA =
        (DirectDrawEnumerateA_type)DetourFindFunction("ddraw.dll", "DirectDrawEnumerateA");
    DetourAttach(&(PVOID &)TrueDirectDrawEnumerateA, HookedDirectDrawEnumerateA);

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    if(DetourIsHelperProcess()) {
        return TRUE;
    }

    if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
        jkgm::setup_default_logging();

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
