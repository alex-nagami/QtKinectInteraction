#include "handprocess.h"

HandProcess::HandProcess(bool _side)
{
  side = _side;
  drawing = false;
  last = HandState_Unknown;
}

QVector2D<Point2f> HandProcess::Process(Point2f pos, HandState state)
{
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
      drawing = false;
      Points userGesture = VP2f2Ps(rightTraj);
      userGesture = DollarOne::Normalize(userGesture);
      userGesture = DollarOne::Resample(userGesture, dollarOne.pointNum);
      QPair<int, double> result = dollarOne.Recognize(userGesture);
      qDebug() << "total" << dollarOne.templates.size() << "match" << result << dollarOne.names[result.first];
      stateMachine.Transfer(dollarOne.names[result.first], side);
    }

    last = state;

    if(drawing)
    {
      rightTraj.push_back(dps[JointType_HandRight]);
      qDebug() << "rightTraj.size()=" << rightTraj.size();
    }
  }
}
