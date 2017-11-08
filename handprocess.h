#ifndef HANDPROCESS_H
#define HANDPROCESS_H

#include <QVector>
#include <QVector2D>

#include <opencv2/opencv.hpp>
#include "EasyKinect.h"
#include "statemachine.h"
#include "dollarone.h"
#include "jointfilter.h"
using namespace cv;

class HandProcess
{
public:
  HandProcess(bool _side=false, StateMachine* sm=nullptr, DollarOne *d=nullptr);

  void DrawTrack(Mat &input, QVector<Point2f> points);
  Mat Process(Point2f pos, HandState state, Mat input);

  QVector<Point2f> track;
  HandState last;
  bool side;
  bool drawing;

  StateMachine *stateMachine;
  DollarOne *dollarOne;
};

#endif // HANDPROCESS_H
