#include <Windows.h>
#include <detours/detours.h>

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    if(DetourIsHelperProcess()) {
        return TRUE;
    }

    if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        // Injected functions

        LONG error = DetourTransactionCommit();

        if(error != NO_ERROR) {
            show_error_message(L"Failed to install renderer");
            return FALSE;
        }
    }
    else if(ul_reason_for_call == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        // Detach injected functions

        LONG error = DetourTransactionCommit();

        if(error != NO_ERROR) {
            show_error_message(L"Failed to uninstall renderer");
            return FALSE;
        }
    }

    return TRUE;
}