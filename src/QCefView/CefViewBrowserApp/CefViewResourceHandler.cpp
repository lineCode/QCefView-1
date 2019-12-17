#include "CefViewResourceHandler.h"
#include <include/wrapper/cef_helpers.h>

CefViewResourceHandler::CefViewResourceHandler()
    : offset_(0) {
}

bool CefViewResourceHandler::Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) {
    handle_request = false;
    return false;
}

void CefViewResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) {
    CEF_REQUIRE_IO_THREAD();
}

bool CefViewResourceHandler::Skip(int64 bytes_to_skip, int64 &bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) {
    bytes_skipped = -2;
    return false;
}

bool CefViewResourceHandler::Read(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefResourceReadCallback> callback) {
    bytes_read = 0;
    if (offset_ < data_.length()) {
        // Copy the next block of data into the buffer.
        int transfer_size = min(bytes_to_read, static_cast<int>(data_.length() - offset_));
        memcpy(data_out, data_.c_str() + offset_, transfer_size);
        offset_ += transfer_size;
        bytes_read = transfer_size;
    }

    return bytes_read > 0;
}

void CefViewResourceHandler::Cancel() {
}
