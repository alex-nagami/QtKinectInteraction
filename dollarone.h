#ifndef DOLLARONE_H
#define DOLLARONE_H

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QVector>
#include <QVector2D>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;

typedef QVector<QVector2D> Points;

const double goldLeft = 1-(sqrt(5)-1)/2;
const double goldRight = (sqrt(5)-1)/2;

class DollarOne
{
public:
  double gestureSize;
  int    pointNum;
  double leftLimit;
  double rightLimit;

  static Points Normalize(Points input, double size = 250);
  static QVector2D BoundingBox(Points input);
  static QVector2D Center(Points input);
  static Points TranslateTo(Points input, const QVector2D center);
  static Points ScaleTo(Points input, double size);
  static Points Rotate(Points input, const double rad);
  static double PathLength(Points input);
  static Points Resample(Points input, const int nums);

  static double Distance(Points a, Points b);
  static double DistanceAtBestAngle(Points a, Points b, double left, double right);

  DollarOne();
  int AddTemplate(Points t, QString name);
  int AddTemplate(QString fileName);
  QPair<int, double> Recognize(Points input);
  bool DeleteTemplate(int index);
  void Clear();

  QVector<Points> templates;
  QVector<QString> names;
};

#endif // DOLLARONE_H
