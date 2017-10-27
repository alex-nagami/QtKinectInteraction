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
  bool CloseGesture();
  bool DrawGesture();
  bool GetSaveGestureFileName(QString);
  bool GetLoadConfigFilenName(QString);
  bool ChangeInput();

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
  Mat infrared;
  KinectBody bodies[BODY_COUNT];

  // left hand filters
  MedianFilter leftMedianX;
  MedianFilter leftMedianY;
  MedianFilter leftMedianZ;
  AverageFilter leftAverageX;
  AverageFilter leftAverageY;
  AverageFilter leftAverageZ;
  MedianFilter leftHandStateMedian;

  // right hanf filters
  MedianFilter rightMedianX;
  MedianFilter rightMedianY;
  MedianFilter rightMedianZ;
  LSEFilter rightLSEX;
  LSEFilter rightLSEY;
  LSEFilter rightLSEZ;
  MedianFilter rightHandStateMedian;

  // right hand state recorder
  QVector<Point2f> rightTraj;
  HandState lastRightHandState = HandState_Unknown;
  bool drawing = false;
};

#endif // VIEWMODEL_H
