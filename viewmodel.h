#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "KinectOpenCvTools.h"
#define _USE_OPENCV
#include "EasyKinect.h"
using namespace cv;
#include "jointfilter.h"
#include "dollarone.h"
#include "statemachine.h"

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
  bool GetOpenGestureFileName(QString);

private slots:
  void TakeFrame();

private:
  void run();

  KinectSensor sensor;
  bool status;
  bool drawingTemplate;
  bool mouseInput = true;
  DollarOne dollarOne;
  StateMachine stateMachine;
};

#endif // VIEWMODEL_H
