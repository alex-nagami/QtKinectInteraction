#include "viewmodel.h"

ViewModel::ViewModel()
{
  HRESULT result;
  result = sensor.init((FrameSourceTypes)(FrameSourceTypes_Depth | FrameSourceTypes_Infrared | FrameSourceTypes_Body));
  if(FAILED(result))
  {
    qDebug() << "Failed to load sensor";
    status = false;
    return;
  }
  status = true;
}

bool ViewModel::GetStatus()
{
  emit Status(status);
  return status;
}

void ViewModel::run()
{
  static int runCount = 0;
//  qDebug() << "running " << runCount++;
  QTimer timer;
  QObject::connect(&timer, &QTimer::timeout, this, &ViewModel::TakeFrame);
  timer.setInterval(30);
  timer.start();

  exec();

  timer.stop();
}

void ViewModel::TakeFrame()
{
  if(!status)
  {
    return;
  }
  static int frameCount = 0;
  HRESULT result = sensor.update();
  if(SUCCEEDED(result))
  {
    Mat infrared = sensor.getInfraredMat();
    if(infrared.size() != Size(0, 0))
    {
      infrared /= 256;
      infrared.convertTo(infrared, CV_8U);
      cvtColor(infrared, infrared, CV_GRAY2RGB);
      qDebug() << "TakeFrame" << frameCount++;
    }
    else
    {
      return;
    }

    KinectBody bodies[BODY_COUNT];
    result = sensor.getKBodyFrame(bodies);
    if(SUCCEEDED(result))
    {
      for(int i=0; i<BODY_COUNT; i++)
      {
        if(bodies[i].tracked)
        {
          qDebug() << "body " << i << "is tracked";
          Point2f dps[JointType_Count];
          for(int jointId=0; jointId<JointType_Count; jointId++)
          {
            DepthSpacePoint p;
            sensor.getMapper()->MapCameraPointToDepthSpace(bodies[i].joints[jointId].Position, &p);
            dps[jointId].x = p.X;
            dps[jointId].y = p.Y;
          }

          infrared = DrawBody(infrared, bodies[i].joints, dps, Scalar(0, 0, 255));
          infrared = DrawHand(infrared, dps[JointType_HandLeft], bodies[i].left, dps[JointType_HandRight], bodies[i].right);
        }
      }
    }

    emit SendDepthFrame(infrared);
  }
}
