#pragma once
#include <include/cef_base.h>
#include <include/cef_browser.h>
#include <include/cef_scheme.h>
#include <QCefProtocol.h>

namespace SchemeHandlerHelper {
    static char *k_scheme_name = QCEF_SCHEMA;

    bool RegisterSchemeHandlerFactory();
    bool RegisterScheme(CefRawPtr<CefSchemeRegistrar> registrar);
}

class SchemeHandlerFactory : public CefSchemeHandlerFactory {
    // Return a new scheme handler instance to handle the request.
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, const CefString &scheme_name, CefRefPtr<CefRequest> request);

private:
    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};

