#ifndef QCEFVIEW_GLOBAL_SETTING_H__
#define QCEFVIEW_GLOBAL_SETTING_H__
#pragma once

#include <include\cef_base.h>

class CefGlobalSetting {
  public:
    static void initializeInstance();
  public:
    static CefString browser_sub_process_path;
    static CefString resource_directory_path;
    static CefString locales_directory_path;
    static CefString user_agent;
    static CefString cache_path;
    static CefString user_data_path;
    static CefString locale;
    static CefString accept_language_list;
    static int persist_session_cookies;
    static int persist_user_preferences;
    static int remote_debugging_port;
    static cef_color_t background_color;
  protected:
    CefGlobalSetting();
    ~CefGlobalSetting();
};
#endif