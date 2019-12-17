#include "CefQueryHandler.h"
#include "Include/QCefQuery.h"
#include "Include/QCefView.h"
#include "QCefViewImplement.h"

CefQueryHandler::CefQueryHandler(QCefViewImplement *pCefViewImpl)
    : pCefViewImpl_(pCefViewImpl){
}

CefQueryHandler::~CefQueryHandler() {
}

bool CefQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          int64 query_id,
                          const CefString &request,
                          bool persistent,
                          CefRefPtr<Callback> callback) {
    if (pCefViewImpl_ && pCefViewImpl_->getCefView()) {
        mtxCallbackMap_.lock();
        mapCallback_[query_id] = callback;
        mtxCallbackMap_.unlock();

        QString strRequest = QString::fromStdString(request.ToString());
        emit pCefViewImpl_->getCefView()->cefQueryRequest(QCefQuery(strRequest, query_id));

        return true;
    }

    return false;
}

void CefQueryHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id) {
    mtxCallbackMap_.lock();
    auto it = mapCallback_.find(query_id);
    if (it != mapCallback_.end())
        mapCallback_.erase(it);

    mtxCallbackMap_.unlock();
}

bool CefQueryHandler::Response(int64_t query, bool success, const CefString &response, int error) {
    CefRefPtr<Callback> cb;
    mtxCallbackMap_.lock();
    auto it = mapCallback_.find(query);
    if (it != mapCallback_.end()) {
        cb = it->second;
        mapCallback_.erase(it);
    }
    mtxCallbackMap_.unlock();

    if (!cb)
        return false;

    if (success)
        cb->Success(response);
    else
        cb->Failure(error, response);

    return true;
}
