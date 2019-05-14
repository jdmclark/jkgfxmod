#include "base/default_logger.hpp"
#include "base/log.hpp"
#include "base/system_string.hpp"
#include "base/win32.hpp"
#include "common/config.hpp"
#include <Windows.h>
#include <detours/detours.h>
#include <string>
#include <regex>

namespace {
    constexpr wchar_t const *appname = L"JkGfxMod";

    void show_error_message(wchar_t const *message)
    {
        MessageBoxW(NULL, message, appname, MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
}

int APIENTRY wWinMain(HINSTANCE /*hInstance*/,
                      HINSTANCE /*hPrevInstance*/,
                      LPWSTR lpCmdLine,
                      int nCmdShow)
{
    try {
        jkgm::setup_default_logging();
        auto cfg = jkgm::load_config_file();

        std::wstring filtered_cmdline = lpCmdLine;

        if(cfg->fullscreen) {
            std::wregex re(L"[/-]windowgui", std::regex_constants::icase);
            filtered_cmdline = std::regex_replace(filtered_cmdline, re, std::wstring());
        }
        else {
            filtered_cmdline.append(L" -windowgui");
        }

        std::wstring cmdbuf = L"jk ";
        cmdbuf.append(filtered_cmdline);

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));

        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = nCmdShow;

        PROCESS_INFORMATION pi;

        BOOL res = DetourCreateProcessWithDllW(jkgm::utf8_to_native(cfg->command).c_str(),
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

        return 0;
    }
    catch(std::exception const &e) {
        LOG_ERROR("Failed to launch JK.exe: ", e.what());
        return 1;
    }
}