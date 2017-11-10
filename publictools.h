#ifndef PUBLICTOOLS_H
#define PUBLICTOOLS_H

#include <opencv2/opencv.hpp>
using namespace cv;
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QVector>
#include <QVector2D>

typedef QVector<QVector2D> Points;

class PublicTools
{
public:
  static Point2f QV2D2P2f(QVector2D p)
  {
    return Point2f(p.x(), p.y());
  }

  static QVector2D P2f2QV2D(Point2f p)
  {
    return QVector2D(p.x, p.y);
  }

  static Points VP2f2Ps(QVector<Point2f> ps)
  {
    Points points;
    for(int i=0; i<ps.size(); i++)
    {
      points.push_back(P2f2QV2D(ps[i]));
    }
    return points;
  }

  static void DrawPoints(Points points, QGraphicsScene* scene)
  {
    if(scene == nullptr) return;
    double xratio = scene->sceneRect().width();
    double yratio = scene->sceneRect().height();
    double wratio = sqrt((xratio*xratio+yratio*yratio)/2);
//    qDebug() << "xratio=" << xratio << "yratio=" << yratio << "wratio=" << wratio;
    const double radius = 0.01;
    const double width = 0.01;

    for(int i=1; i<points.size(); i++)
    {
      scene->addLine(
          points[i-1].x()*xratio, points[i-1].y()*yratio,
          points[i].x()*xratio, points[i].y()*yratio,
          QPen(QColor(0, 0, 255), width*wratio));
    }

    for(int i=0; i<points.size(); i++)
    {
      scene->addEllipse(
            points[i].x()*xratio-radius*wratio/2, points[i].y()*yratio-radius*wratio/2,
            radius*wratio, radius*wratio,
            QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)));
    }
  }
};

#endif // PUBLICTOOLS_H
