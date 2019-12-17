#include "DpiHelper.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>

#ifndef DPI_ENUMS_DECLARED
    typedef enum PROCESS_DPI_AWARENESS {
        PROCESS_DPI_UNAWARE = 0,
        PROCESS_SYSTEM_DPI_AWARE = 1,
        PROCESS_PER_MONITOR_DPI_AWARE = 2
    } PROCESS_DPI_AWARENESS;

    typedef enum MONITOR_DPI_TYPE {
        MDT_EFFECTIVE_DPI = 0,
        MDT_ANGULAR_DPI = 1,
        MDT_RAW_DPI = 2,
        MDT_DEFAULT = MDT_EFFECTIVE_DPI
    } MONITOR_DPI_TYPE;

    #define DPI_ENUMS_DECLARED
#endif // (DPI_ENUMS_DECLARED)

typedef HRESULT(WINAPI *LPSetProcessDpiAwareness)(
    _In_ PROCESS_DPI_AWARENESS value
    );

typedef HRESULT(WINAPI *LPGetProcessDpiAwareness)(
    _In_  HANDLE                hprocess,
    _Out_ PROCESS_DPI_AWARENESS *value
    );


typedef HRESULT(WINAPI *LPGetDpiForMonitor)(
    _In_  HMONITOR         hmonitor,
    _In_  MONITOR_DPI_TYPE dpiType,
    _Out_ UINT             *dpiX,
    _Out_ UINT             *dpiY
    );

int GetDPIOfMonitor(HMONITOR hMonitor) {
    UINT dpix = 96, dpiy = 96;
    HRESULT  hr = E_FAIL;
    HMODULE hModule = ::LoadLibraryW(L"Shcore.dll");
    if (hModule != NULL) {
        LPGetDpiForMonitor GetDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");

        if (GetDpiForMonitor != NULL && GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy) != S_OK) {
            return 96;
        }
    } else {
        HDC screen = GetDC(0);
        dpix = GetDeviceCaps(screen, LOGPIXELSX);
        ReleaseDC(0, screen);
    }

    return dpix;
}

int GetDPIOfMonitorNearestToPoint(POINT pt) {
    HMONITOR hMonitor;
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    return GetDPIOfMonitor(hMonitor);
}

int GetMainMonitorDPI() {
    POINT    pt;
    // Get the DPI for the main monitor
    pt.x = 1;
    pt.y = 1;
    return GetDPIOfMonitorNearestToPoint(pt);
}

BOOL GetDPIAwareness(PROCESS_DPI_AWARENESS* pAwareness) {
    BOOL bRet = FALSE;
    HMODULE hModule = ::LoadLibrary(TEXT("Shcore.dll"));

    if (hModule != NULL) {
        LPGetProcessDpiAwareness GetProcessDpiAwareness = (LPGetProcessDpiAwareness)GetProcAddress(hModule, "GetProcessDpiAwareness");

        if (GetProcessDpiAwareness != NULL) {
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());

            if (GetProcessDpiAwareness(hProcess, pAwareness) == S_OK) {
                bRet = TRUE;
            }
        }
    }

    return bRet;
}

#endif

float getDpiScaleFactor() {
    float scaleFactor = 1.f;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    PROCESS_DPI_AWARENESS dpiAwareness;
    if (GetDPIAwareness(&dpiAwareness)) {
        scaleFactor = GetMainMonitorDPI() / 96.f;
    }
#endif
    return scaleFactor;
}
