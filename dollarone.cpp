#include "dollarone.h"

DollarOne::DollarOne()
{
  pointNum = 32;
  gestureSize = 250;
  leftLimit = -M_PI/4;
  rightLimit = M_PI/4;
}

Points DollarOne::Normalize(Points input, double size)
{
  return ScaleTo(TranslateTo(input, QVector2D(0, 0)), size);
}

QVector2D DollarOne::BoundingBox(Points input)
{
  double xmin = 1e20, xmax = -1e20;
  double ymin = 1e20, ymax = -1e20;
  for(int i=0; i<input.size(); i++)
  {
    if(input[i].x()<xmin) xmin = input[i].x();
    if(input[i].x()>xmax) xmax = input[i].x();

    if(input[i].y()<ymin) ymin = input[i].y();
    if(input[i].y()>ymax) ymax = input[i].y();
  }
  return QVector2D(xmax-xmin, ymax-ymin);
}

QVector2D DollarOne::Center(Points input)
{
  double xmin = 1e20, xmax = -1e20;
  double ymin = 1e20, ymax = -1e20;
  for(int i=0; i<input.size(); i++)
  {
    if(input[i].x()<xmin) xmin = input[i].x();
    if(input[i].x()>xmax) xmax = input[i].x();

    if(input[i].y()<ymin) ymin = input[i].y();
    if(input[i].y()>ymax) ymax = input[i].y();
  }
  return QVector2D((xmin+xmax)/2, (ymin+ymax)/2);
}

Points DollarOne::TranslateTo(Points input, const QVector2D center)
{
  QVector2D c = Center(input);
  for(int i=0; i<input.size(); i++)
  {
    input[i] += center - c;
  }
  return input;
}

Points DollarOne::ScaleTo(Points input, double size)
{
  QVector2D c = Center(input);
  QVector2D s = BoundingBox(input);
  for(int i=0; i<input.size(); i++)
  {
    input[i].setX(input[i].x()*size/s.x());
    input[i].setY(input[i].y()*size/s.y());
  }
  TranslateTo(input, c);
  return input;
}

Points DollarOne::Rotate(Points input, const double rad)
{
  for(int i=0; i<input.size(); i++)
  {
    double x = input[i].x();
    double y = input[i].y();
    input[i] = QVector2D(x*cos(rad)-y*sin(rad), x*sin(rad)+y*cos(rad));
  }
  return input;
}

double DollarOne::PathLength(Points input)
{
  double sum = 0;
  for(int i=0; i<input.size()-1; i++)
  {
    sum+=(input[i+1]-input[i]).length();
  }
  return sum;
}

Points DollarOne::Resample(Points input, const int nums)
{
  if(input.size()==0) return input;
  if(input.size()==1) return Points(nums, input[0]);
  double averageEdgeLen = PathLength(input) / (nums-1);
  double nowLen = 0;
  Points result;
  result.push_back(input[0]);
  for(int i=1; i<input.size(); i++)
  {
    double thisEdge = (input[i]-input[i-1]).length();
    if(nowLen+thisEdge>=averageEdgeLen-1e-6)
    {
      double r1 = (averageEdgeLen - nowLen)/thisEdge;
      QVector2D newP = r1*input[i]+(1-r1)*input[i-1];
      result.push_back(newP);
      input.insert(i, newP);
      nowLen = 0;
    }
    else
    {
      nowLen+=thisEdge;
    }
  }
  if(result.size()<nums)
    result.push_back(input[input.size()-1]);
  return result;
}

double DollarOne::Distance(Points a, Points b)
{
  qDebug() << "a.size()=" << a.size() << " b.size()=" << b.size();
  if(a.size()!=b.size()) return -1;
  double sum = 0;
  for(int i=0; i<a.size(); i++)
  {
    sum+=(a[i]-b[i]).length();
  }
  sum/=a.size();
  return sum;
}

double DollarOne::DistanceAtBestAngle(Points a, Points b, double left, double right)
{
  if(fabs(right-left)<M_PI*1/180)
  {
    return Distance(a, b);
  }
  double lm = goldRight*left+goldLeft*right;
  double rm = goldLeft*left+goldRight*right;

  Points tl = Rotate(a, lm);
  Points tr = Rotate(a, rm);

  double dl = Distance(tl, b);
  double dr = Distance(tr, b);

  if(dl<dr)
  {
    return DistanceAtBestAngle(a, b, left, rm);
  }
  else
  {
    return DistanceAtBestAngle(a, b, lm, right);
  }
}

int DollarOne::AddTemplate(Points t, QString name)
{
  t = Normalize(t, gestureSize);
  t = Resample(t, pointNum);
  templates.push_back(t);
  names.push_back(name);
  return templates.size()-1;
}

QPair<int, double> DollarOne::Recognize(Points input)
{
  input = Normalize(input, gestureSize);
  input = Resample(input, pointNum);

  double min = 1e20;
  int minn = -1;
  for(int i=0; i<templates.size(); i++)
  {
    double score = DistanceAtBestAngle(input, templates[i], leftLimit, rightLimit);
    if(score<min)
    {
      min = score;
      minn = i;
    }
  }

  return QPair<int, double>(minn, min/(0.5*sqrt(2*gestureSize*gestureSize)));
}

bool DollarOne::DeleteTemplate(int index)
{
  if(index<0 || index>=templates.size())
  {
    return false;
  }
  templates.remove(index);
  names.remove(index);
  return true;
}

void DollarOne::Clear()
{
  templates.clear();
}
