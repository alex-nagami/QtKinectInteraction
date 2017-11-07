#ifndef HANDPROCESS_H
#define HANDPROCESS_H

#include <QVector>
#include <QVector2D>

#include <opencv2/opencv.hpp>
#include "EasyKinect.h"
using namespace cv;

class HandProcess
{
public:
  HandProcess(bool _side=false);

  QVector<Point2f> Process(Point2f pos, HandState state);

  QVector<Point2f> track;
  HandState last;
  bool side;
};

#endif // HANDPROCESS_H
