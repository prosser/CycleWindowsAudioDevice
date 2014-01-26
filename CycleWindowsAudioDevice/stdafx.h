// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <mmdeviceapi.h>
#include <initguid.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmreg.h>

#define EXIT_ON_ERROR(hres)  \
    if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
    if ((punk) != NULL)  \
        { (punk)->Release(); (punk) = NULL; }

#define SAFE_FREE_ARRAY(p,c) \
    {if (p) {\
    for (size_t i = 0; i < c; i++){\
    if (!p[i]) break;\
    CoTaskMemFree(p[i]);\
    }\
    CoTaskMemFree(p);}}
