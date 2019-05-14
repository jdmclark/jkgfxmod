#include "error_reporter.hpp"
#include "base/log.hpp"
#include "base/system_string.hpp"
#include <Windows.h>

namespace {
    constexpr wchar_t const *appname = L"JkGfxMod";

    void error_dialog(std::string_view message)
    {
        MessageBoxW(NULL,
                    jkgm::utf8_to_native(message).c_str(),
                    appname,
                    MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
}

void jkgm::report_error_message(std::string_view msg)
{
    error_dialog(msg);
}

void jkgm::report_unimplemented_function(std::string_view function_name)
{
    LOG_ERROR(function_name, " unimplemented");
    error_dialog(str(format("The program called an undefined function in JkGfxMod. The program may "
                            "not be compatible with JkGfxMod, or it may have been modified in an "
                            "unsupported way.\n\nDetails: Failed to call \"",
                            function_name,
                            "\", because it is not implemented")));
    abort();
}