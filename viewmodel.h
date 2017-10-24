#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "KinectOpenCvTools.h"
#define _USE_OPENCV
#include "EasyKinect.h"
using namespace cv;
#include "jointfilter.h"

class ViewModel : public QThread
{
  Q_OBJECT
public:
  ViewModel();

signals:
  void Status(bool);
  void SendDepthFrame(Mat);
  void SendGestureFrame(Mat);

public slots:
  bool GetStatus();

private slots:
  void TakeFrame();

private:
  void run();

  KinectSensor sensor;
  bool status;
};

#endif // VIEWMODEL_H
