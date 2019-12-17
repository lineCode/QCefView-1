#include <algorithm>
#include <string>
#include <include/cef_browser.h>
#include <include/cef_callback.h>
#include <include/cef_frame.h>
#include <include/cef_resource_handler.h>
#include <include/cef_response.h>
#include <include/cef_request.h>
#include <include/cef_scheme.h>
#include <include/wrapper/cef_helpers.h>
#include "CefViewSchemeHandler.h"
#include "CefViewResourceHandler.h"

namespace SchemeHandlerHelper {
    bool RegisterScheme(CefRawPtr<CefSchemeRegistrar> registrar) {
        return registrar->AddCustomScheme(k_scheme_name, 0);
    }

    bool RegisterSchemeHandlerFactory() {
        return CefRegisterSchemeHandlerFactory(k_scheme_name, "", new SchemeHandlerFactory());
    }
}

CefRefPtr<CefResourceHandler>
SchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString &scheme_name,
    CefRefPtr<CefRequest> request) {
    return new CefViewResourceHandler();
}