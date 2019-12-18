#include "TransparentCefWnd.h"
#include <QMouseEvent>
#include <QDebug>

TransparentCefWnd::TransparentCefWnd(QWidget *parent)
    : QMainWindow(parent)
    , leftMousePressed_(false) {
    ui.setupUi(this);
    this->setWindowTitle("Transparent CEF");
    this->setFixedWidth(1024);
    this->setFixedHeight(768);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    ui.widgetCef->navigateToUrl(QCoreApplication::applicationDirPath() + "/tree.html");
    qInfo() << ui.widgetCef->size();
}

TransparentCefWnd::~TransparentCefWnd() {

}

void TransparentCefWnd::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        leftMousePressed_ = true;
        beginDragPos_ = event->globalPos() - this->frameGeometry().topLeft();
    }
    return QWidget::mousePressEvent(event);
}

void TransparentCefWnd::mouseMoveEvent(QMouseEvent *event) {
    QPoint globalPoint = event->globalPos();
    if (leftMousePressed_) {
        move(event->globalPos() - beginDragPos_);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void TransparentCefWnd::mouseReleaseEvent(QMouseEvent *event) {
    leftMousePressed_ = false;
    return QWidget::mouseReleaseEvent(event);
}
