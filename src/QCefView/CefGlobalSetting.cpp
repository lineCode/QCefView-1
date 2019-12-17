#include <QCoreApplication>
#include <QDir>

#include <QCefProtocol.h>

#include "CefGlobalSetting.h"

CefString CefGlobalSetting::browser_sub_process_path;

CefString CefGlobalSetting::resource_directory_path;

CefString CefGlobalSetting::locales_directory_path;

CefString CefGlobalSetting::user_agent;

CefString CefGlobalSetting::cache_path;

CefString CefGlobalSetting::user_data_path;

int CefGlobalSetting::persist_session_cookies;

int CefGlobalSetting::persist_user_preferences;

CefString CefGlobalSetting::locale;

int CefGlobalSetting::remote_debugging_port;

cef_color_t CefGlobalSetting::background_color;

CefString CefGlobalSetting::accept_language_list;

void CefGlobalSetting::initializeInstance() {
  static CefGlobalSetting s_instance;
}

CefGlobalSetting::CefGlobalSetting()
{
  QDir ExeDir = QCoreApplication::applicationDirPath();

  QString strExePath = ExeDir.filePath(RENDER_PROCESS_NAME);
  browser_sub_process_path.FromString(QDir::toNativeSeparators(strExePath).toStdString());

  QString strResPath = ExeDir.filePath(RESOURCE_DIRECTORY_NAME);
  resource_directory_path.FromString(QDir::toNativeSeparators(strResPath).toStdString());

  QDir ResPath(strResPath);
  locales_directory_path.FromString(QDir::toNativeSeparators(ResPath.filePath(LOCALES_DIRECTORY_NAME)).toStdString());

  user_agent.FromString(QCEF_USER_AGENT);

  accept_language_list.FromWString(L"zh-CN,en-US,en");
  locale.FromWString(L"zh-CN");
}

CefGlobalSetting::~CefGlobalSetting() {

}
