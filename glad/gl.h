#pragma once

#ifdef PLATFORM_WINDOWS
#define APIENTRY __stdcall
#endif

#include "glad.h"

/*#if __has_include(<windows.h>)
#error "windows.h is included"
#endif*/
