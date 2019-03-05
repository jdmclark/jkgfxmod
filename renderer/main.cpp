#include "base/default_logger.hpp"
#include "base/log.hpp"
#include <Windows.h>
#include <detours/detours.h>

constexpr wchar_t const *appname = L"JkGfxMod";

void show_error_message(wchar_t const *message);
bool attach_hooks();
bool detach_hooks();

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

#include <Windows.h>
#include <detours/detours.h>

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

    LONG error = DetourTransactionCommit();
    return error == NO_ERROR;
}

bool detach_hooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Detach injected functions

    LONG error = DetourTransactionCommit();
    return error == NO_ERROR;
}