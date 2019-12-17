#include <include/cef_sandbox_win.h>
#include "CefManager.h"
#include "CefGlobalSetting.h"

CefManager::CefManager() {
    nBrowserRefCount_ = 0;
}

CefManager::~CefManager() {

}

CefManager &CefManager::getInstance() {
    static CefManager s_instance;
    return s_instance;
}

void CefManager::initializeCef() {
    // This is not the first time initialization
    if (++nBrowserRefCount_ > 1)
        return;

    // Enable High-DPI support on Windows 7 or newer.
    CefEnableHighDPISupport();

    // This is the first time initialization
    CefGlobalSetting::initializeInstance();

    CefString(&cef_settings_.browser_subprocess_path) = CefGlobalSetting::browser_sub_process_path;
    CefString(&cef_settings_.resources_dir_path) = CefGlobalSetting::resource_directory_path;
    CefString(&cef_settings_.locales_dir_path) = CefGlobalSetting::locales_directory_path;
    CefString(&cef_settings_.user_agent) = CefGlobalSetting::user_agent;
    CefString(&cef_settings_.cache_path) = CefGlobalSetting::cache_path;
    CefString(&cef_settings_.user_data_path) = CefGlobalSetting::user_data_path;
    CefString(&cef_settings_.locale) = CefGlobalSetting::locale;
    CefString(&cef_settings_.accept_language_list) = CefGlobalSetting::accept_language_list;

    cef_settings_.persist_session_cookies = CefGlobalSetting::persist_session_cookies;
    cef_settings_.persist_user_preferences = CefGlobalSetting::persist_user_preferences;
    cef_settings_.remote_debugging_port = CefGlobalSetting::remote_debugging_port;
    cef_settings_.background_color = CefGlobalSetting::background_color;
    cef_settings_.no_sandbox = true;
    cef_settings_.pack_loading_disabled = false;
    cef_settings_.multi_threaded_message_loop = true;
    cef_settings_.windowless_rendering_enabled = true;

#ifndef NDEBUG
    cef_settings_.log_severity = LOGSEVERITY_INFO;
    cef_settings_.remote_debugging_port = 7777;
#else
    cef_settings_.log_severity = LOGSEVERITY_DISABLE;
#endif

    app_ = new CefViewBrowserApp();

    // TODO: Linux
    HINSTANCE hInstance = ::GetModuleHandle(nullptr);
    CefMainArgs main_args(hInstance);

    // Initialize CEF.
    void *sandboxInfo = nullptr;
    if (!CefInitialize(main_args, cef_settings_, app_, sandboxInfo))
        assert(0);
}

void CefManager::uninitializeCef() {
    // This is not the last time release
    if (--nBrowserRefCount_ > 0)
        return;

    // The last time release
    // TO-DO (sheen) when we reach here, it is possible there are pending
    // IO requests, and they will fire the DCHECK when complete or aborted
    releaseCef();

    // Destroy the application
    app_ = nullptr;
}

void CefManager::releaseCef() {
    CefShutdown();
}
