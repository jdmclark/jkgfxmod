#include <Windows.h>
#include <detours/detours.h>
#include <string>

constexpr wchar_t const *appname = L"JkGfxMod";

void show_error_message(wchar_t const *message)
{
    MessageBoxW(NULL, message, appname, MB_OK);
}

int APIENTRY wWinMain(HINSTANCE /*hInstance*/,
                      HINSTANCE /*hPrevInstance*/,
                      LPWSTR lpCmdLine,
                      int nCmdShow)
{
    std::wstring cmdbuf = L"jk ";
    cmdbuf.append(lpCmdLine);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = nCmdShow;

    PROCESS_INFORMATION pi;

    BOOL res = DetourCreateProcessWithDllW(L"jk.exe",
                                           &cmdbuf[0],
                                           /*security attributes*/ NULL,
                                           /*thread attributes*/ NULL,
                                           /*inherit handles*/ FALSE,
                                           /*creation flags*/ 0U,
                                           /*environment*/ NULL,
                                           /*current dir*/ NULL,
                                           /*startup info*/ &si,
                                           /*process info*/ &pi,
                                           /*injected DLL name*/ "renderer.dll",
                                           /*create proc routine*/ NULL);
    if(res == FALSE) {
        show_error_message(L"Failed to launch JK.exe");
        return 1;
    }

    auto wait_res = WaitForSingleObject(pi.hProcess, INFINITE);
    if(wait_res != WAIT_OBJECT_0) {
        show_error_message(L"Failed to wait for JK.exe to exit");
        return 1;
    }

    DWORD exit_code = 0U;
    auto ec_res = GetExitCodeProcess(pi.hProcess, &exit_code);
    if(ec_res == FALSE) {
        show_error_message(L"Failed to get JK.exe exit code");
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exit_code;
}