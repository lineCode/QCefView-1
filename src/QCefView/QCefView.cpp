#include "Include/QCefView.h"
#include <QResizeEvent>
#include <QVariant>
#include <QAbstractEventDispatcher>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_sandbox_win.h>
#include "QCefViewImplement.h"
#include <QDebug>


QCefView::QCefView(QWidget *parent /*= nullptr*/)
    : QWidget(parent) {
    pImpl_ = std::make_unique<QCefViewImplement>(this);
    setAttribute(Qt::WA_NativeWindow, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setMouseTracking(true);
}

QCefView::~QCefView() {
    disconnect();

    if (pImpl_) {
        pImpl_->closeAllBrowsers();
        pImpl_.reset();
    }
}

void QCefView::navigateToUrl(const QString &url) {
    if (pImpl_)
        pImpl_->navigateToUrl(url);
}

bool QCefView::browserCanGoBack() {
    if (pImpl_)
        return pImpl_->browserCanGoBack();

    return false;
}

bool QCefView::browserCanGoForward() {
    if (pImpl_)
        return pImpl_->browserCanGoForward();

    return false;
}

void QCefView::browserGoBack() {
    if (pImpl_)
        pImpl_->browserGoBack();
}

void QCefView::browserGoForward() {
    if (pImpl_)
        pImpl_->browserGoForward();
}

bool QCefView::browserIsLoading() {
    if (pImpl_)
        return pImpl_->browserIsLoading();

    return false;
}

void QCefView::browserReload() {
    if (pImpl_)
        pImpl_->browserReload();
}

void QCefView::browserStopLoad() {
    if (pImpl_)
        pImpl_->browserStopLoad();
}

bool QCefView::triggerEvent(const QString &name, const QCefEvent &event) {
    if (pImpl_)
        return pImpl_->triggerEvent(name, event, CefViewBrowserHandler::MAIN_FRAME);

    return false;
}

bool QCefView::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
    if (pImpl_)
        return pImpl_->triggerEvent(name, event, frameId);

    return false;
}

bool QCefView::broadcastEvent(const QString &name, const QCefEvent &event) {
    if (pImpl_)
        return pImpl_->triggerEvent(name, event, CefViewBrowserHandler::ALL_FRAMES);

    return false;
}

bool QCefView::responseQCefQuery(const QCefQuery &query) {
    if (pImpl_)
        return pImpl_->responseQCefQuery(query);

    return false;
}

bool QCefView::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    if(pImpl_)
        return pImpl_->nativeEvent(eventType, message, result);
    return false;
}

void QCefView::paintEvent(QPaintEvent *event) {
    if(pImpl_)
        pImpl_->paintEvent(event);
}