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
  void Status(Status);
  void SendDepthFrame(Mat);
  void SendGestureFrame(Mat);

public slots:
  Status GetStatus();
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

  // status bits
  enum Status
  {
    Status_Error = -1,
    Status_ShowUserHand = 0,
    Status_ShowTemplate = 1,
    Status_DrawTemplate = 2
  };

  bool inited;
  Status status;
  bool mouseInput = true;

  KinectSensor sensor;
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

  DollarOne dollarOne;
  StateMachine stateMachine;

  QVector<Point2f> gestureTemplate;
};

#endif // VIEWMODEL_H
