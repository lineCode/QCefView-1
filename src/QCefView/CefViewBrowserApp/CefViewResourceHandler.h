#pragma once

#include "include/cef_resource_handler.h"

class CefViewResourceHandler : public CefResourceHandler {
public:
    CefViewResourceHandler();

    virtual bool Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) override;

    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64 &response_length,
        CefString &redirectUrl) override;

    virtual bool Skip(int64 bytes_to_skip, int64 &bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) override;

    virtual bool Read(void *data_out,
        int bytes_to_read,
        int &bytes_read,
        CefRefPtr<CefResourceReadCallback> callback) override;

    virtual void Cancel() override;

private:
    std::string data_;
    std::string mime_type_;
    int offset_;

private:
    IMPLEMENT_REFCOUNTING(CefViewResourceHandler);
};