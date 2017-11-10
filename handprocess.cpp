#include "handprocess.h"

static Point2f QV2D2P2f(QVector2D p) { return Point2f(p.x(), p.y()); }
static QVector2D P2f2QV2D(Point2f p) { return QVector2D(p.x, p.y); }
static Points VP2f2Ps(QVector<Point2f> ps)
{
  Points points;
  for(int i=0; i<ps.size(); i++)
  {
    points.push_back(P2f2QV2D(ps[i]));
  }
  return points;
}

HandProcess::HandProcess(bool _side, StateMachine *sm, DollarOne *d)
{
  side = _side;
  drawing = false;
  last = HandState_Unknown;
  stateMachine = sm;
  dollarOne = d;
}

void HandProcess::DrawTrack(Mat &input, QVector<Point2f> points)
{
  for(int i=0; i<points.size()-1; i++)
  {
//              qDebug() << "drawing line #" << i;
    line(input, points[i], points[i+1], Scalar::all(0), 2);
  }
  for(int i=0; i<points.size(); i++)
  {
//              qDebug() << "drawing point #" << i;
    circle(input, points[i], 2, Scalar(255, 0, 0), -1);
  }
}

Mat HandProcess::Process(Point2f pos, HandState state, Mat input)
{
  Mat result;
  if(stateMachine == nullptr || dollarOne == nullptr) return result;
  if(state == HandState_NotTracked || state == HandState_Unknown)
  {
    state = last;
  }

  if(state != HandState_NotTracked && state != HandState_Unknown)
  {
    if(last != HandState_Closed && state == HandState_Closed)
    {
      drawing = true;
      track.clear();
    }
    if(last == HandState_Closed && (state == HandState_Open || state == HandState_Lasso))
    {
      qDebug() << "track ended";
      drawing = false;
      Points userGesture = VP2f2Ps(track);
      qDebug() << "into ps";
      QPair<int, double> result = dollarOne->Recognize(userGesture);
      if(result.first>=0 && result.first<dollarOne->names.size())
      {
        qDebug() << "total" << dollarOne->templates.size() << "match" << result << dollarOne->names[result.first];
      }
      if(result.second < 0.4)
      {
        stateMachine->Transfer(dollarOne->names[result.first], side);
      }
    }

    last = state;

    if(drawing)
    {
      track.push_back(pos);
      qDebug() << "track.size()=" << track.size();
    }
  }

  if(!input.empty())
  {
    result = input.clone();
    DrawTrack(result, track);
  }
  return result;
}
