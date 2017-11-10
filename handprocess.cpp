#include "handprocess.h"

HandProcess::HandProcess(bool _side, StateMachine *sm, DollarOne *d)
{
  side = _side;
  drawing = false;
  last = HandState_Unknown;
  stateMachine = sm;
  dollarOne = d;
}

void HandProcess::Process(Point2f pos, HandState state, QGraphicsScene* scene)
{
  if(stateMachine == nullptr || dollarOne == nullptr) return;
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
      paint.clear();
    }
    if(last == HandState_Closed && (state == HandState_Open || state == HandState_Lasso))
    {
      qDebug() << "track ended";
      drawing = false;
      Points userGesture = PublicTools::VP2f2Ps(track);
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
      paint.push_back(Point2f(pos.x/512.0, pos.y/424.0));
      qDebug() << "track.size()=" << track.size();
    }
  }

  PublicTools::DrawPoints(PublicTools::VP2f2Ps(paint), scene);
}
