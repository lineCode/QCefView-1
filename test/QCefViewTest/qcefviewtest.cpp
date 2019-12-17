#include "qcefviewtest.h"
#include <QDebug>
#include "TransparentCefWnd.h"

QCefViewTest::QCefViewTest(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);

    connect(ui.lineEditUrl, &QLineEdit::returnPressed, [this]() {
        ui.widgetCef->navigateToUrl(ui.lineEditUrl->text());
    });

    connect(ui.pushButtonGoBack, &QPushButton::clicked, [this]() {
        ui.widgetCef->browserGoBack();
    });

    connect(ui.pushButtonGoForward, &QPushButton::clicked, [this]() {
        ui.widgetCef->browserGoForward();
    });

    connect(ui.pushButtonReloadOrStop, &QPushButton::clicked, [this]() {
        if (ui.pushButtonReloadOrStop->text() == "Stop")
            ui.widgetCef->browserStopLoad();
        else if (ui.pushButtonReloadOrStop->text() == "Reload")
            ui.widgetCef->browserReload();
    });

    connect(ui.pushButtonTriggerEvent, &QPushButton::clicked, [this]() {
        QCefEvent event("test");
        event.setStringProperty("str_prop", "test string");
        event.setIntProperty("int_prop", 1234);
        event.setDoubleProperty("double_prop", 3.14f);
        event.setBoolProperty("bool_prop", true);

        ui.widgetCef->broadcastEvent("eventTest", event);
    });

    connect(ui.pushButtonTransparent, &QPushButton::clicked, [this]() {
        TransparentCefWnd* pTransparentCefWnd = new TransparentCefWnd();
        pTransparentCefWnd->show();
    });

    connect(ui.widgetCef, &QCefView::loadingStateChanged, this, [this](bool isLoading, bool canGoBack, bool canGoForward) {
        if (isLoading) {
            ui.pushButtonReloadOrStop->setText("Stop");
        } else {
            ui.pushButtonReloadOrStop->setText("Reload");
        }

        ui.pushButtonGoBack->setEnabled(canGoBack);
        ui.pushButtonGoForward->setEnabled(canGoForward);
    }, Qt::QueuedConnection);


    connect(ui.widgetCef, &QCefView::invokeMethodNotify, this, [this](int browserId, int frameId, const QString &method, const QVariantList &arguments) {
        qInfo() << "[invokeMethodNotify] " << "browserId: " << browserId << ", frameId: " << frameId << ", method: " << method << ", arguments: " << arguments;
    }, Qt::QueuedConnection);


    connect(ui.widgetCef, &QCefView::cefUrlRequest, this, [this](const QString &url) {
        qInfo() << "[cefUrlRequest] " << url;
    }, Qt::QueuedConnection);


    connect(ui.widgetCef, &QCefView::cefQueryRequest, this, [this](const QCefQuery &query) {
        qInfo() << "[cefQueryRequest] " <<
            "id: " << query.id() <<
            ", reqeust:" << query.reqeust() <<
            ", response: " << query.response() <<
            ", result: " << query.result() <<
            ", error: " << query.error();
    }, Qt::QueuedConnection);


    ui.widgetCef->navigateToUrl(QCoreApplication::applicationDirPath() + "/QCefViewTestPage.html");
}

QCefViewTest::~QCefViewTest() {
}
