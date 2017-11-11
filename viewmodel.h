#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QDebug>
#include <QFile>
#include <QFileInfo>
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
#include "handprocess.h"

class ViewModel : public QThread
{
  Q_OBJECT
public:
  ViewModel();
  // status bits
  enum Status
  {
    Status_Error = -1,
    Status_ShowUserHand = 0,
    Status_ShowTemplate = 1,
    Status_DrawTemplate = 2
  };

signals:
  void SigStatus(Status);
  void SendDepthFrame(Mat);
  void SendGestureFrame(Mat);
  void SendGestureScene(QGraphicsScene*);
  void SendLoadConfigError(ErrorInfo);

public slots:
  bool GetOpenGestureFileName(QString);
  void CloseGesture();
  void DrawGesture();
  void DrawGesturePoint(QVector<QVector2D> point);
  bool GetSaveGestureFileName(QString);
  bool GetLoadConfigFileName(QString);

private slots:
  void TakeFrame();

private:
  bool inited;
  Status status;
  bool mouseInput = true;
  bool drawing = false;

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

  // hand processer
  HandProcess left;
  HandProcess right;

  DollarOne dollarOne;
  StateMachine stateMachine;

  Points gestureTemplate;
  Points drawingGesture;

  void run();
};

#endif // VIEWMODEL_H
