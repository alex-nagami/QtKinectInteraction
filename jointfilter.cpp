#include "jointfilter.h"

// class JointFilter
Filter::Filter()
{

}

double Filter::Process(double xt)
{
  return xt;
}

void Filter::Clear()
{

}

// MedianJointFilter
MedianFilter::MedianFilter(int _size)
{
  if(_size<1) _size = 1;
  size = _size;
}

double MedianFilter::Process(double xt)
{
  data.push_back(xt);
  while(data.size()>size)
  {
    data.removeFirst();
  }
//  qDebug() << "data.size()=" << data.size();
  QVector<double> temp = data;
  qSort(temp);
  if(data.size()%2)
  {
    return temp[data.size()/2];
  }
  else
  {
    return (temp[data.size()/2-1]+temp[data.size()/2])/2;
  }
}

void MedianFilter::Clear()
{
  data.clear();
}

// AverageFilter
AverageFilter::AverageFilter(int _size)
{
  if(_size<1) _size = 1;
  size = _size;
}

double AverageFilter::Process(double xt)
{
  data.push_back(xt);
  while(data.size()>size)
  {
    data.removeFirst();
  }
  double sum = 0;
  for(int i=0; i<data.size(); i++)
  {
    sum+=data[i];
  }

  return sum/data.size();
}

void AverageFilter::Clear()
{
  data.clear();
}

// LSEFilter
LSEFilter::LSEFilter(int _size, int _rank)
{
  if(_size<2) _size = 2;
  if(_rank<1) _rank = 1;
  size = _size;
  rank = _rank;
  A = Mat(rank+1, rank+1, CV_64F, Scalar::all(0));
  P = Mat(1, rank+1, CV_64F, Scalar::all(0));
  for(int i=0; i<rank+1; i++)
  {
    for(int j=0; j<rank+1; j++)
    {
      for(int k=0; k<size; k++)
      {
        A.at<double>(i, j) += pow(k, i+j);
      }
    }
    P.at<double>(0, i) = pow(size-1, i);
  }
}

double LSEFilter::Process(double xt)
{
  data.push_back(xt);
  while(data.size()>size)
  {
    data.removeFirst();
  }
  if(data.size()==size)
  {
    Mat b(rank+1, 1, CV_64F, Scalar::all(0));
    for(int i=0; i<rank+1; i++)
    {
      for(int j=0; j<size; j++)
      {
        b.at<double>(i, 0) += pow(j, i)*xt;
      }
    }

    Mat a = A.inv()*b;
    Mat result = P*a;
    return result.at<double>(0, 0);
  }
  return -10086;
}

void LSEFilter::Clear()
{
  data.clear();
}
