#ifndef QCEFEVENT_H
#define QCEFEVENT_H
#pragma once

#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>

#ifdef QCEFVIEW_LIB
    #define QCEFVIEW_EXPORT Q_DECL_EXPORT
#else
    #define QCEFVIEW_EXPORT Q_DECL_IMPORT
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        #pragma comment(lib, "QCefView.lib")
    #endif
#endif


class QCEFVIEW_EXPORT QCefEvent :
    public QObject {
    Q_OBJECT
  public:
    /// <summary>
    ///
    /// </summary>
    QCefEvent();

    /// <summary>
    ///
    /// </summary>
    /// <param name="name"></param>
    QCefEvent(const char *name);

    /// <summary>
    ///
    /// </summary>
    /// <param name="name"></param>
    void setEventName(const char *name);

    /// <summary>
    ///
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    void setIntProperty(const char *key, int value);

    /// <summary>
    ///
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    void setDoubleProperty(const char *key, double value);

    /// <summary>
    ///
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    void setStringProperty(const char *key, QString value);

    /// <summary>
    ///
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    void setBoolProperty(const char *key, bool value);
};

#endif // QCEFEVENT_H
