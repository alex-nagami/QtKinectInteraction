#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
  void Exit();

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void GetDepthFrame(Mat);
  void GetGestureFrame(Mat);

private:
  // items for depth display
  QGraphicsPixmapItem* itemDepth;
  QGraphicsScene* sceneDepth;

  // items for gesture display
  QGraphicsPixmapItem* itemGesture;
  QGraphicsScene* sceneGesture;

  Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
