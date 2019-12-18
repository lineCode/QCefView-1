#include "CefViewBrowserHandler.h"
#include <sstream>
#include <string>
#include <include/cef_app.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#include <QRect>
#include <QWidget>
#include <QVariant>
#include <QDebug>
#include <QCefProtocol.h>
#include "QCefViewImplement.h"

CefViewBrowserHandler::CefViewBrowserHandler(QCefViewImplement *pCefViewImpl)
    : isClosing_(false)
    , pCefqueryHandler_(new CefQueryHandler(pCefViewImpl))
    , pResourceManager_(new CefResourceManager())
    , pMessageRouter_(nullptr)
    , browserCount_(0)
    , pCefViewImpl_(pCefViewImpl) {
}

CefViewBrowserHandler::~CefViewBrowserHandler() {

}

bool CefViewBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    if (pMessageRouter_ && pMessageRouter_->OnProcessMessageReceived(browser, frame, source_process, message))
        return true;

    if (DispatchNotifyRequest(browser, source_process, message))
        return true;

    return false;
}

void CefViewBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model) {
    CEF_REQUIRE_UI_THREAD();

    model->Clear();
}

bool CefViewBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int command_id,
    EventFlags event_flags) {
    CEF_REQUIRE_UI_THREAD();

    return false;
}

void CefViewBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &url) {
    CEF_REQUIRE_UI_THREAD();
}

void CefViewBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title) {
    CEF_REQUIRE_UI_THREAD();
}

bool CefViewBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
    cef_log_severity_t level,
    const CefString &message,
    const CefString &source,
    int line) {
    CEF_REQUIRE_UI_THREAD();
    if (source.empty() || message.empty())
        return false;

    std::string src = source.ToString();
    std::size_t found = src.find_last_of("/\\");
    if (found != std::string::npos && found < src.length() - 1)
        src = src.substr(found + 1);

    __noop(src, message.ToString());
    return false;
}

bool CefViewBrowserHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDragData> dragData,
    CefDragHandler::DragOperationsMask mask) {
    CEF_REQUIRE_UI_THREAD();

    return true;
}

bool CefViewBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
    const CefString &origin_url,
    JSDialogType dialog_type,
    const CefString &message_text,
    const CefString &default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool &suppress_message) {
    CEF_REQUIRE_UI_THREAD();

    return false;
}

bool CefViewBrowserHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
    const CefString &message_text,
    bool is_reload,
    CefRefPtr<CefJSDialogCallback> callback) {
    CEF_REQUIRE_UI_THREAD();

    return false;
}

void CefViewBrowserHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
}

bool CefViewBrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
    const CefKeyEvent &event,
    CefEventHandle os_event,
    bool *is_keyboard_shortcut) {
    CEF_REQUIRE_UI_THREAD();

    return false;
}

bool CefViewBrowserHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString &target_url,
    const CefString &target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures &popupFeatures,
    CefWindowInfo &windowInfo,
    CefRefPtr<CefClient> &client,
    CefBrowserSettings &settings,
    CefRefPtr<CefDictionaryValue> &extra_info,
    bool *no_javascript_access) {
    CEF_REQUIRE_UI_THREAD();

    if (isClosing_)
        return true;

    frame->LoadURL(target_url);
    return true;
}

void CefViewBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    if (isClosing_)
        return;

    if (!pMessageRouter_) {
        // Create the browser-side router for query handling.
        CefMessageRouterConfig config;
        config.js_query_function = QCEF_QUERY_NAME;
        config.js_cancel_function = QCEF_QUERY_CANCEL_NAME;

        // Register handlers with the router.
        pMessageRouter_ = CefMessageRouterBrowserSide::Create(config);
        pMessageRouter_->AddHandler(pCefqueryHandler_.get(), false);
    }

    if (!pMainBrowser_) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            pMainBrowser_ = browser;
        }

        pCefViewImpl_->browserCreatedNotify(browser);
    } else if (browser->IsPopup()) {
        popupBrowserList_.push_back(browser);
        CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true)));
    }

    {
        std::unique_lock<std::mutex> lock(mtx_);
        browserCount_++;
    }
}

bool CefViewBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    if (pMainBrowser_ && pMainBrowser_->IsSame(browser))
        isClosing_ = true;

    return false;
}

void CefViewBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    if (pMessageRouter_)
        pMessageRouter_->OnBeforeClose(browser);

    if (pMainBrowser_ && pMainBrowser_->IsSame(browser)) {
        // if the main browser is closing, we need to close all the pop up browsers.
        if (!popupBrowserList_.empty()) {
            for (auto &popup : popupBrowserList_) {
                if (popup) {
                    popup->StopLoad();
                    popup->GetHost()->CloseBrowser(true);
                }
            }
        }

        pMainBrowser_->StopLoad();
        pMainBrowser_ = nullptr;
    } else if (browser->IsPopup()) {
        // Remove from the browser popup list.
        for (auto it = popupBrowserList_.begin(); it != popupBrowserList_.end(); ++it) {
            if ((*it)->IsSame(browser)) {
                popupBrowserList_.erase(it);
                break;
            }
        }
    }

    // Decrease the browser count
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (--browserCount_ == 0) {
            if (pMessageRouter_) {
                pMessageRouter_->RemoveHandler(pCefqueryHandler_.get());
                pCefqueryHandler_ = nullptr;
                pMessageRouter_ = nullptr;
            }
            closeCV_.notify_all();
        }
    }
}

void CefViewBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
    bool isLoading,
    bool canGoBack,
    bool canGoForward) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();
    if (pCefView)
        emit pCefView->loadingStateChanged(isLoading, canGoBack, canGoForward);
}

void CefViewBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    TransitionType transition_type) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();
    if (pCefView)
        emit pCefView->loadStart();
}

void CefViewBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();
    if (pCefView)
        emit pCefView->loadEnd(httpStatusCode);
}

void CefViewBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString &errorText,
    const CefString &failedUrl) {
    CEF_REQUIRE_UI_THREAD();
    if (errorCode == ERR_ABORTED)
        return;

    QString msg = QString::fromStdString(errorText.ToString());
    QString url = QString::fromStdString(failedUrl.ToString());

    QCefView* pCefView = GetCefView();
    if (pCefView)
        emit pCefView->loadError(errorCode, msg, url);
}

bool CefViewBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool user_gesture,
    bool is_redirect) {
    CEF_REQUIRE_UI_THREAD();

    if (pMessageRouter_)
        pMessageRouter_->OnBeforeBrowse(browser, frame);
    return false;
}

bool CefViewBrowserHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString &target_url,
    CefRequestHandler::WindowOpenDisposition target_disposition,
    bool user_gesture) {
    CEF_REQUIRE_UI_THREAD();

    return false; // return true to cancel this navigation.
}

CefRefPtr<CefResourceRequestHandler> CefViewBrowserHandler::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString &request_initiator,
    bool &disable_default_handling) {
    CEF_REQUIRE_IO_THREAD();
    return this;
}

bool CefViewBrowserHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
    const CefString &origin_url,
    int64 new_size,
    CefRefPtr<CefRequestCallback> callback) {
    CEF_REQUIRE_IO_THREAD();
    static const int maxSize = 10 * 1024 * 1024;
    callback->Continue(new_size <= maxSize);
    return true;
}

void CefViewBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) {
    CEF_REQUIRE_UI_THREAD();

    if (pMessageRouter_)
        pMessageRouter_->OnRenderProcessTerminated(browser);

    if (pMainBrowser_ && pMainBrowser_->GetMainFrame()) {
        CefString url = pMainBrowser_->GetMainFrame()->GetURL();
        if (!url.empty()) {
            pMainBrowser_->GetMainFrame()->LoadURL(url);
        }
    }
}

CefResourceRequestHandler::ReturnValue CefViewBrowserHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefRequestCallback> callback) {
    return pResourceManager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> CefViewBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
    return pResourceManager_->GetResourceHandler(browser, frame, request);
}

void CefViewBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool &allow_os_execution) {
}

bool CefViewBrowserHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    CEF_REQUIRE_UI_THREAD();
    return false;
}

bool CefViewBrowserHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int &screenX, int &screenY) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();

    DCHECK(pCefView);
    if (!pCefView)
        return false;

    QPoint p = { (int)(viewX * pCefViewImpl_->deviceScaleFactor()), (int)(viewY * pCefViewImpl_->deviceScaleFactor()) };
    QPoint global_p = pCefView->mapToGlobal(p);

    screenX = global_p.x();
    screenY = global_p.y();

    return true;
}

void CefViewBrowserHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();

    DCHECK(pCefView);
    if (!pCefView)
        return;

    QRect viewRect = pCefView->rect();
    rect.x = 0;
    rect.y = 0;
    rect.width = viewRect.width();
    rect.height = viewRect.height();

    //qInfo() << "GetViewRect:" << rect.width << ", " << rect.height;
}

bool CefViewBrowserHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();

    DCHECK(pCefView);
    if (!pCefView)
        return false;

    CefRect view_rect;
    GetViewRect(browser, view_rect);

    screen_info.device_scale_factor = pCefViewImpl_->deviceScaleFactor();
    screen_info.rect = view_rect;
    screen_info.available_rect = view_rect;
    return true;
}

void CefViewBrowserHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {
}

void CefViewBrowserHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect &rect) {
}

void CefViewBrowserHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects,
    const void *buffer, int width, int height) {

        {
            std::lock_guard<std::mutex> lg(imageMtx_);
            size_t newSize = width * height * 4;

            if (newSize != drawImageParam_.imageHeight * drawImageParam_.imageWidth)
                drawImageParam_.imageArray.reset(new uint8_t[newSize]);

            memcpy(drawImageParam_.imageArray.get(), buffer, newSize);

            drawImageParam_.imageWidth = width;
            drawImageParam_.imageHeight = height;
        }

        if (pCefViewImpl_)
            pCefViewImpl_->updateView();
}

void CefViewBrowserHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor,
    CursorType type,
    const CefCursorInfo &custom_cursor_info) {
    CEF_REQUIRE_UI_THREAD();
    QCefView* pCefView = GetCefView();
    if (!pCefView)
        return;
    SetClassLongPtr((HWND)pCefView->winId(), GCLP_HCURSOR, static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
    ::SetCursor(cursor);
}

void CefViewBrowserHandler::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range, const CefRenderHandler::RectList &character_bounds) {
    if (pCefViewImpl_)
        pCefViewImpl_->OnImeCompositionRangeChanged(browser, selection_range, character_bounds);
}

CefRefPtr<CefBrowser> CefViewBrowserHandler::GetBrowser() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return pMainBrowser_;
}

void CefViewBrowserHandler::CloseAllBrowsers(bool force_close) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (browserCount_ == 0)
        return;

    isClosing_ = true;

    // Close all popup browsers if any
    if (!popupBrowserList_.empty()) {
        for (auto it = popupBrowserList_.begin(); it != popupBrowserList_.end(); ++it)
            (*it)->GetHost()->CloseBrowser(force_close);
    }

    if (pMainBrowser_) {
        pMainBrowser_->GetHost()->CloseBrowser(force_close);
    }

    closeCV_.wait(lock);
}

bool CefViewBrowserHandler::TriggerEvent(const int64_t frame_id, const CefRefPtr<CefProcessMessage> msg) {
    if (msg->GetName().empty())
        return false;

    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser) {
        std::vector<int64> frameIds;
        if (MAIN_FRAME == frame_id) {
            frameIds.push_back(browser->GetMainFrame()->GetIdentifier());
        } else if (ALL_FRAMES == frame_id) {
            browser->GetFrameIdentifiers(frameIds);
        } else {
            frameIds.push_back(frame_id);
        }

        for (auto id : frameIds) {
            auto frame = browser->GetFrame(id);
            frame->SendProcessMessage(PID_RENDERER, msg);
            return true;
        }
    }

    return false;
}

bool CefViewBrowserHandler::ResponseQuery(const int64_t query, bool success, const CefString &response, int error) {
    if (pCefqueryHandler_)
        return pCefqueryHandler_->Response(query, success, response, error);

    return false;
}

bool CefViewBrowserHandler::DispatchNotifyRequest(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
    if (message->GetName() == INVOKEMETHOD_NOTIFY_MESSAGE) {
        CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
        if (messageArguments && (messageArguments->GetSize() >= 2)) {
            size_t idx = 0;
            if (CefValueType::VTYPE_INT == messageArguments->GetType(idx)) {
                int browserId = browser->GetIdentifier();
                int frameId = messageArguments->GetInt(idx++);

                if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
                    CefString functionName = messageArguments->GetString(idx++);
                    if (functionName == QCEF_INVOKEMETHOD) {
                        QString method;
                        if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
                            method = QString::fromWCharArray(messageArguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
                            method = QString::fromUtf8(messageArguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
                            method = QString::fromWCharArray(messageArguments->GetString(idx++).c_str());
#endif
                        }

                        QVariantList arguments;
                        QString qStr;
                        for (; idx < messageArguments->GetSize(); idx++) {
                            if (CefValueType::VTYPE_NULL == messageArguments->GetType(idx))
                                arguments.push_back(0);
                            else if (CefValueType::VTYPE_BOOL == messageArguments->GetType(idx))
                                arguments.push_back(QVariant::fromValue(messageArguments->GetBool(idx)));
                            else if (CefValueType::VTYPE_INT == messageArguments->GetType(idx))
                                arguments.push_back(QVariant::fromValue(messageArguments->GetInt(idx)));
                            else if (CefValueType::VTYPE_DOUBLE == messageArguments->GetType(idx))
                                arguments.push_back(QVariant::fromValue(messageArguments->GetDouble(idx)));
                            else if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
                                qStr = QString::fromWCharArray(messageArguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
                                qStr = QString::fromUtf8(messageArguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
                                qStr = QString::fromWCharArray(messageArguments->GetString(idx).c_str());
#endif
                                arguments.push_back(qStr);
                            } else {
                                DCHECK(false);
                            }
                        }

                        QCefView* pCefView = GetCefView();
                        if (pCefView)
                            emit pCefView->invokeMethodNotify(browserId, frameId, method, arguments);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

DrawImageParam* CefViewBrowserHandler::lockImage() {
    imageMtx_.lock();
    return &drawImageParam_;
}

void CefViewBrowserHandler::releaseImage() {
    imageMtx_.unlock();
}

QCefView *CefViewBrowserHandler::GetCefView() {
    if (!pCefViewImpl_)
        return nullptr;
    return pCefViewImpl_->getCefView();
}
