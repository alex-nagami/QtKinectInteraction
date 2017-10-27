#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
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
  void SigOpenGesture(QString);
  void SigCloseGesture();
  void SigDrawGesture();
  void SigSaveGesture(QString);
  void SigLoadConfig(QString);
  void SigChangeInput();

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void GetDepthFrame(Mat);
  void GetGestureFrame(Mat);

private slots:
  void on_buttonOpenGesture_clicked();

  void on_buttonCloseGesture_clicked();

  void on_buttonDrawGesture_clicked();

  void on_buttonSaveGesture_clicked();

  void on_buttonLoadConfig_clicked();

  void on_buttonChangeInput_clicked();

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
