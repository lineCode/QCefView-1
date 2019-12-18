#ifndef QCEFVIEW_H
#define QCEFVIEW_H
#pragma once

#include <memory>
#include <QtCore/qglobal.h>
#include <QWidget>
#include <QOpenGLWidget>
#include <QVariantList>
#include <QAbstractNativeEventFilter>
#include "QCefQuery.h"
#include "QCefEvent.h"

#ifdef QCEFVIEW_LIB
    #define QCEFVIEW_EXPORT Q_DECL_EXPORT
#else
    #define QCEFVIEW_EXPORT Q_DECL_IMPORT
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        #pragma comment(lib, "QCefView.lib")
    #endif
#endif


class QCefViewImplement;

class QCEFVIEW_EXPORT QCefView :
    public QWidget {
    Q_OBJECT
  public:
    QCefView(QWidget *parent = nullptr);

    ~QCefView();

    void navigateToUrl(const QString &url);

    bool browserCanGoBack();

    bool browserCanGoForward();

    void browserGoBack();

    void browserGoForward();

    bool browserIsLoading();

    void browserReload();

    void browserStopLoad();

    bool triggerEvent(const QString &name, const QCefEvent &event);

    bool triggerEvent(const QString &name, const QCefEvent &event, int frameId);

    bool broadcastEvent(const QString &name, const QCefEvent &event);

    bool responseQCefQuery(const QCefQuery &query);

    void setFPS(int fps);
    int fps() const;
  protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
  signals:
    /// <summary>
    ///
    /// </summary>
    /// <param name="isLoading"></param>
    /// <param name="canGoBack"></param>
    /// <param name="canGoForward"></param>
    void loadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward);

    /// <summary>
    ///
    /// </summary>
    void loadStart();

    /// <summary>
    ///
    /// </summary>
    /// <param name="httpStatusCode"></param>
    void loadEnd(int httpStatusCode);

    /// <summary>
    ///
    /// </summary>
    /// <param name="errorCode"></param>
    /// <param name="errorMsg"></param>
    /// <param name="failedUrl"></param>
    void loadError(int errorCode, const QString &errorMsg, const QString &failedUrl);

    /// <summary>
    ///
    /// </summary>
    /// <param name="url"></param>
    void cefUrlRequest(const QString &url);

    /// <summary>
    ///
    /// </summary>
    /// <param name="query"></param>
    void cefQueryRequest(const QCefQuery &query);

    /// <summary>
    ///
    /// </summary>
    /// <param name="browserId"></param>
    /// <param name="frameId"></param>
    /// <param name="method"></param>
    /// <param name="arguments"></param>
    void invokeMethodNotify(int browserId, int frameId, const QString &method, const QVariantList &arguments);

  protected:
    void paintEvent(QPaintEvent *event) override;
  private:
    std::unique_ptr<QCefViewImplement> pImpl_;
};

#endif // QCEFVIEW_H
