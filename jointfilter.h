#ifndef JOINTFILTER_H
#define JOINTFILTER_H

#include <QDebug>
#include <QVector>
#include <opencv2/opencv.hpp>
using namespace cv;

class Filter
{
public:
  Filter();

  virtual double Process(double);
  virtual void Clear();
};

class MedianFilter : public Filter
{
public:
  MedianFilter(int _size = 5);

  double Process(double) override;
  void Clear() override;

private:
  QVector<double> data;
  int size;
};

class AverageFilter : public Filter
{
public:
  AverageFilter(int _size = 3);

  double Process(double) override;
  void Clear() override;

private:
  QVector<double> data;
  int size;
};

class LSEFilter : public Filter
{
public:
  LSEFilter(int _size = 4, int _rank = 2);

  double Process(double) override;
  void Clear() override;

private:
  Mat A;
  Mat P;
  QVector<double> data;
  int size;
  int rank;
};

#endif // JOINTFILTER_H
