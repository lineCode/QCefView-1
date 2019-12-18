#include "QCefViewImplement.h"
#include "CefManager.h"
#include "QCefProtocol.h"
#include <QDebug>
#include <QApplication>
#include <QPainter>
#include "DpiHelper.h"

QCefViewImplement::QCefViewImplement(QCefView *pCefView)
    : pCefView_(pCefView)
    , hwnd_(nullptr)
    , browserCreated_(false)
    , pQCefViewHandler_(nullptr)
    , fps_(60) {
    deviceScaleFactor_ = getDpiScaleFactor();
}

QCefViewImplement::~QCefViewImplement() {

}

void QCefViewImplement::createBrowser() {
    if (browserCreated_)
        return;
    if (!pCefView_)
        return;
    hwnd_ = (HWND)pCefView_->winId();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    RegisterTouchWindow(hwnd_, 0);
#endif

    CefWindowInfo window_info;
    window_info.SetAsWindowless(hwnd_);

    CefBrowserSettings browserSettings;
    browserSettings.plugins = STATE_DISABLED; // disable all plugins
    browserSettings.windowless_frame_rate = fps_;

    CefManager::getInstance().initializeCef();

    pQCefViewHandler_ = new CefViewBrowserHandler(this);

    if (!CefBrowserHost::CreateBrowser(window_info,
        pQCefViewHandler_,
        "",
        browserSettings,
        nullptr,
        CefRequestContext::GetGlobalContext())) {
        return;
    }

    browserCreated_ = true;
}

void QCefViewImplement::closeAllBrowsers() {
    pCefUIEventWin_.reset();

    if (pQCefViewHandler_) {
        pQCefViewHandler_->CloseAllBrowsers(true);
        pQCefViewHandler_ = nullptr;
    }

    CefManager::getInstance().uninitializeCef();
    browserCreated_ = false;
}

void QCefViewImplement::browserCreatedNotify(CefRefPtr<CefBrowser> browser) {
    if (pCefUIEventWin_)
        pCefUIEventWin_.reset();
    pCefUIEventWin_ = std::make_shared<UIEventHandlerWin>(hwnd_, deviceScaleFactor_, browser);

    if (cachedNavigateUrl_.length() > 0) {
        browser->GetMainFrame()->LoadURL(cachedNavigateUrl_);
    }
    cachedNavigateUrl_.clear();
}

void QCefViewImplement::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range, const CefRenderHandler::RectList &character_bounds) {
    if (pCefUIEventWin_) {
        pCefUIEventWin_->OnImeCompositionRangeChanged(browser, selection_range, character_bounds);
    }
}

void QCefViewImplement::navigateToUrl(const QString &url) {
    if (!browserCreated_)
        createBrowser();

    CefString strUrl;
    strUrl.FromString(url.toStdString());

    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser() && pQCefViewHandler_->GetBrowser()->GetMainFrame()) {
        pQCefViewHandler_->GetBrowser()->GetMainFrame()->LoadURL(strUrl);
    } else {
        cachedNavigateUrl_ = strUrl;
    }
}

bool QCefViewImplement::browserCanGoBack() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        return pQCefViewHandler_->GetBrowser()->CanGoBack();

    return false;
}

bool QCefViewImplement::browserCanGoForward() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        return pQCefViewHandler_->GetBrowser()->CanGoForward();

    return false;
}

void QCefViewImplement::browserGoBack() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        pQCefViewHandler_->GetBrowser()->GoBack();
}

void QCefViewImplement::browserGoForward() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        pQCefViewHandler_->GetBrowser()->GoForward();
}

bool QCefViewImplement::browserIsLoading() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        return pQCefViewHandler_->GetBrowser()->IsLoading();

    return false;
}

void QCefViewImplement::browserReload() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        pQCefViewHandler_->GetBrowser()->Reload();
}

void QCefViewImplement::browserStopLoad() {
    if (pQCefViewHandler_ && pQCefViewHandler_->GetBrowser())
        pQCefViewHandler_->GetBrowser()->StopLoad();
}

bool QCefViewImplement::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
    if (!name.isEmpty()) {
        return sendEventNotifyMessage(frameId, name, event);
    }

    return false;
}

bool QCefViewImplement::responseQCefQuery(const QCefQuery &query) {
    if (pQCefViewHandler_) {
        CefString res;
        res.FromString(query.response().toStdString());
        return pQCefViewHandler_->ResponseQuery(query.id(), query.result(), res, query.error());
    }
    return false;
}

bool QCefViewImplement::sendEventNotifyMessage(int frameId, const QString &name, const QCefEvent &event) {
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(TRIGGEREVENT_NOTIFY_MESSAGE);
    CefRefPtr<CefListValue> arguments = msg->GetArgumentList();

    int idx = 0;
    CefString eventName = name.toStdString();
    arguments->SetString(idx++, eventName);

    CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();

    CefString cefStr;
    cefStr.FromWString(event.objectName().toStdWString());
    dict->SetString("name", cefStr);

    QList<QByteArray> keys = event.dynamicPropertyNames();
    for (QByteArray key : keys) {
        QVariant value = event.property(key.data());
        if (value.type() == QMetaType::Bool)
            dict->SetBool(key.data(), value.toBool());
        else if (value.type() == QMetaType::Int || value.type() == QMetaType::UInt)
            dict->SetInt(key.data(), value.toInt());
        else if (value.type() == QMetaType::Double)
            dict->SetDouble(key.data(), value.toDouble());
        else if (value.type() == QMetaType::QString) {
            cefStr.FromWString(value.toString().toStdWString());
            dict->SetString(key.data(), cefStr);
        } else {
        }
    }

    arguments->SetDictionary(idx++, dict);

    return pQCefViewHandler_->TriggerEvent(frameId, msg);
}

bool QCefViewImplement::nativeEvent(const QByteArray &eventType, void *message, long *result) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    if (eventType == "windows_generic_MSG") {
        MSG *pMsg = (MSG *)message;
        if (pCefUIEventWin_ && pMsg) {
            if (pMsg->message == WM_SYSCHAR || pMsg->message == WM_SYSKEYDOWN ||
                pMsg->message == WM_SYSKEYUP || pMsg->message == WM_KEYDOWN ||
                pMsg->message == WM_KEYUP || pMsg->message == WM_CHAR) {
                if (pCefView_->isActiveWindow() && pCefView_->hasFocus()) {
                    pCefUIEventWin_->OnKeyboardEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
                }
            } else if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN ||
                pMsg->message == WM_LBUTTONUP || pMsg->message == WM_RBUTTONUP || pMsg->message == WM_MBUTTONUP ||
                pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_MOUSELEAVE || pMsg->message == WM_MOUSEWHEEL) {

                if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN) {
                    pCefView_->setFocus();
                }

                if (pCefView_->isActiveWindow()) {
                    pCefUIEventWin_->OnMouseEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
                }
            } else if (pMsg->message == WM_SIZE) {
                pCefUIEventWin_->OnSize(pMsg->message, pMsg->wParam, pMsg->lParam);
            } else if (pMsg->message == WM_TOUCH) {
                pCefUIEventWin_->OnTouchEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
            } else if (pMsg->message == WM_SETFOCUS || pMsg->message == WM_KILLFOCUS) {
                pCefUIEventWin_->OnFocusEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
            } else if (pMsg->message == WM_IME_SETCONTEXT || pMsg->message == WM_IME_STARTCOMPOSITION ||
                pMsg->message == WM_IME_COMPOSITION || pMsg->message == WM_IME_ENDCOMPOSITION) {
                if (pCefView_->isActiveWindow() && pCefView_->hasFocus()) {
                    pCefUIEventWin_->OnIMEEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
                }
            }
        }
    }
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#elif TARGET_OS_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
#elif __unix__
#elif defined(_POSIX_VERSION)
#else
#error "Unknown compiler"
#endif
    return false;
}

CefRefPtr<CefBrowserHost> QCefViewImplement::getCefBrowserHost() {
    if (!pQCefViewHandler_)
        return nullptr;

    CefRefPtr<CefBrowser> browser = pQCefViewHandler_->GetBrowser();
    if (!browser)
        return nullptr;

    return browser->GetHost();
}

QCefView *QCefViewImplement::getCefView() {
    return pCefView_;
}

void QCefViewImplement::paintEvent(QPaintEvent *event) {
    if (!pQCefViewHandler_)
        return;
    DrawImageParam* pDrawImageParam = pQCefViewHandler_->lockImage();
    if (pDrawImageParam) {
        QImage image(pDrawImageParam->imageArray.get(), pDrawImageParam->imageWidth, pDrawImageParam->imageHeight, QImage::Format_ARGB32);
        //static int i = 0;
        //image.save(QString("D:\\test_img\\%1.bmp").arg(i++));
        QPainter paint(pCefView_);
        paint.drawImage(pCefView_->rect(), image);
    }
    pQCefViewHandler_->releaseImage();
}

float QCefViewImplement::deviceScaleFactor() {
    return deviceScaleFactor_.load();
}

void QCefViewImplement::setFPS(int fps) {
    fps_ = fps;
}

int QCefViewImplement::fps() const {
    return fps_;
}

void QCefViewImplement::updateView() {
    if (pCefView_)
        pCefView_->update();
}
