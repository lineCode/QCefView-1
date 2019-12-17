#ifndef TRANSPARENT_WND_H__
#define TRANSPARENT_WND_H__

#include <QtWidgets/QMainWindow>
#include "ui_transparent.h"

class TransparentCefWnd : public QMainWindow {
    Q_OBJECT

  public:
      TransparentCefWnd(QWidget *parent = 0);
    ~TransparentCefWnd();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
  private:
    Ui::TransparentCefWnd ui;
    bool leftMousePressed_;
    QPoint beginDragPos_;
};

#endif // TRANSPARENT_WND_H__
