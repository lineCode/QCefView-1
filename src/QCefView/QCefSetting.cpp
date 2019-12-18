#include "Include/QCefSetting.h"
#include "CefGlobalSetting.h"

#include <QColor.h>

void
QCefSetting::setBrowserSubProcessPath(const QString& path)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::browser_sub_process_path.FromString(path.toStdString());
}

const QString
QCefSetting::browserSubProcessPath()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::browser_sub_process_path.ToString());
}

void
QCefSetting::setResourceDirectoryPath(const QString& path)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::resource_directory_path.FromString(path.toStdString());
}

const QString
QCefSetting::resourceDirectoryPath()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::resource_directory_path.ToString());
}

void
QCefSetting::setLocalesDirectoryPath(const QString& path)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::locales_directory_path.FromString(path.toStdString());
}

const QString
QCefSetting::localesDirectoryPath()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::locales_directory_path.ToString());
}

void
QCefSetting::setUserAgent(const QString& agent)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::user_agent.FromString(agent.toStdString());
}

const QString
QCefSetting::userAgent()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::user_agent.ToString());
}

void
QCefSetting::setCachePath(const QString& path)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::cache_path.FromString(path.toStdString());
}

const QString
QCefSetting::cachePath()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::cache_path.ToString());
}

void
QCefSetting::setUserDataPath(const QString& path)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::user_data_path.FromString(path.toStdString());
}

const QString
QCefSetting::userDataPath()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::user_data_path.ToString());
}

void
QCefSetting::setPersistSessionCookies(bool enabled)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::persist_session_cookies = enabled ? TRUE : FALSE;
}

const bool
QCefSetting::persistSessionCookies()
{
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::persist_session_cookies ? true : false;
}

void
QCefSetting::setPersistUserPreferences(bool enabled)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::persist_user_preferences = enabled ? TRUE : FALSE;
}

const bool
QCefSetting::persistUserPreferences()
{
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::persist_user_preferences ? true : false;
}

void
QCefSetting::setLocale(const QString& locale)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::locale.FromString(locale.toStdString());
}

const QString
QCefSetting::locale()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::locale.ToString());
}

void
QCefSetting::setRemoteDebuggingPort(int port)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::remote_debugging_port = port;
}

const int
QCefSetting::remoteDebuggingPort()
{
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::remote_debugging_port;
}

void
QCefSetting::setBackgroundColor(const QColor& color)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::background_color = static_cast<cef_color_t>(color.rgba());
}

const QColor
QCefSetting::backgroundColor()
{
  CefGlobalSetting::initializeInstance();
  return QColor::fromRgba(static_cast<QRgb>(CefGlobalSetting::background_color));
}

void
QCefSetting::setAcceptLanguageList(const QString& languages)
{
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::accept_language_list.FromString(languages.toStdString());
}

const QString
QCefSetting::acceptLanguageList()
{
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::accept_language_list.ToString());
}

const void QCefSetting::setEnableGPU(bool b) {
    CefGlobalSetting::initializeInstance();
    CefGlobalSetting::enable_gpu = b;
}

const bool QCefSetting::enableGPU() {
    return CefGlobalSetting::enable_gpu;
}
