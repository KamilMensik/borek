#pragma once

#ifdef BR_PLATFORM_WINDOWS
        #ifdef BR_BUILD_DLL
                #define BOREK_API __declspec(dllexport)
        #else
                #define BOREK_API __declspec(dllimport)
        #endif
#else
        #define BOREK_API
#endif
