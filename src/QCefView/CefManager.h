#pragma once

#include "CefViewBrowserApp/CefViewBrowserApp.h"

class CefManager {
  public:
    static CefManager &getInstance();
    void initializeCef();
    void uninitializeCef();
  protected:
    CefManager();
    ~CefManager();
  protected:
    void releaseCef();
  private:
    CefRefPtr<CefViewBrowserApp> app_;
    CefSettings cef_settings_;
    int64_t nBrowserRefCount_;
};
