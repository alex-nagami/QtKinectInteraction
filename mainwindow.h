#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
using namespace cv;
#include "publictools.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
  void Exit();
  void SigOpenGesture(QString);
  void SigCloseGesture();
  void SigDrawGesture();
  void SigSaveGesture(QString);
  void SigLoadConfig(QString);
  void SigChangeInput();
  void SigDrawPoint(QVector<QVector2D>);

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void GetDepthFrame(Mat);
  void GetGestureFrame(Mat);
  void GetGestureScene(QGraphicsScene* scene);

private slots:
  void on_buttonOpenGesture_clicked();

  void on_buttonCloseGesture_clicked();

  void on_buttonDrawGesture_clicked();

  void on_buttonSaveGesture_clicked();

  void on_buttonLoadConfig_clicked();

  void on_buttonChangeInput_clicked();

  void on_buttonExit_clicked();

  void on_gvGesture_MouseEvent(QMouseEvent *event);

  void on_gvGesture_MousePressEvent(QMouseEvent *);

  void on_gvGesture_MouseReleaseEvent(QMouseEvent *);

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

private:
  // items for depth display
  QGraphicsPixmapItem* itemDepth;
  QGraphicsScene* sceneDepth;

  // items for gesture display
  QGraphicsPixmapItem* itemGesture;
  QGraphicsScene* sceneGesture;

  Ui::MainWindow *ui;
  bool mouseDrawing;
  QVector<QVector2D> drawPoints;

  QPoint mouseOrigin;
};

#endif // MAINWINDOW_H
