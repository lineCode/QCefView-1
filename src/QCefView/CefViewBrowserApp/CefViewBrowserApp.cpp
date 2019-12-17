#include "CefViewBrowserApp.h"
#include <string>
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#include "CefViewSchemeHandler.h"

CefViewBrowserApp::CefViewBrowserApp() {
}

CefViewBrowserApp::~CefViewBrowserApp() {
}

void CefViewBrowserApp::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) {
    command_line->AppendSwitch("disable-spell-checking");
    command_line->AppendSwitch("disable-extensions");
    command_line->AppendSwitch("disable-pdf-extension");
    command_line->AppendSwitch("enable-direct-write");
    command_line->AppendSwitch("allow-file-access-from-files");
    command_line->AppendSwitch("no-proxy-server");
    command_line->AppendSwitch("in-process-gpu");
    command_line->AppendSwitch("disable-direct-composition");
    command_line->AppendSwitchWithValue("disable-features", "NetworkService");
    command_line->AppendSwitchWithValue("renderer-process-limit", "1");


    //command_line->AppendSwitch("no-proxy-server");
    //command_line->AppendSwitch("disable-extensions");
    //command_line->AppendSwitch("disable-surfaces");
    //command_line->AppendSwitch("disable-gpu-shader-disk-cache");

    //// D3D11模式不能禁用GPU加速，2623版本不会使用D3D11模式
    //command_line->AppendSwitch("disable-gpu");
    //command_line->AppendSwitch("disable-gpu-compositing");
}

void CefViewBrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    // TODO
    //SchemeHandlerHelper::RegisterScheme(registrar);
}

CefRefPtr<CefResourceBundleHandler> CefViewBrowserApp::GetResourceBundleHandler() {
    return nullptr;
}

CefRefPtr<CefBrowserProcessHandler> CefViewBrowserApp::GetBrowserProcessHandler() {
    return this;
}

CefRefPtr<CefRenderProcessHandler> CefViewBrowserApp::GetRenderProcessHandler() {
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
void CefViewBrowserApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    // Register cookieable schemes with the global cookie manager.
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
    DCHECK(manager.get());    
    manager->SetSupportedSchemes(cookieable_schemes_, true, nullptr);

    // TODO
    //SchemeHandlerHelper::RegisterSchemeHandlerFactory();
}

void CefViewBrowserApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) {

}

void CefViewBrowserApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) {
    CEF_REQUIRE_IO_THREAD();
}

CefRefPtr<CefPrintHandler> CefViewBrowserApp::GetPrintHandler() {
    return nullptr;
}

void CefViewBrowserApp::OnScheduleMessagePumpWork(int64 delay_ms) {
}