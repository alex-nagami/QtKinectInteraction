#include "viewmodel.h"

ViewModel::ViewModel()
{
  HRESULT result;
  result = sensor.init((FrameSourceTypes)(FrameSourceTypes_Depth | FrameSourceTypes_Infrared | FrameSourceTypes_Body));
  if(FAILED(result))
  {
    qDebug() << "Failed to load sensor";
    inited = false;
    return;
  }
  inited = true;
}

Status ViewModel::GetStatus()
{
  emit Status(status);
  return status;
}

bool ViewModel::GetOpenGestureFileName(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }
  QTextStream in(&file);
  Points temp;
  while(!in.atEnd())
  {
    double x, y;
    in >> x >> y;
    temp.push_back(QVector2D(x, y));
  }
  displayingTemplate = true;
  return true;
}

bool ViewModel::CloseGesture()
{
  status = Status_ShowUserHand;
  gestureTemplate.clear();
}

bool ViewModel::GetOpenGestureFileName(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }
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

// Main data process here
void ViewModel::TakeFrame()
{
  if(!inited)
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
//      qDebug() << "TakeFrame" << frameCount++;
    }
    else
    {
      return;
    }

    result = sensor.getKBodyFrame(bodies);
    if(SUCCEEDED(result))
    {
      for(int i=0; i<BODY_COUNT; i++)
      {
        if(bodies[i].tracked)
        {
//          qDebug() << "body " << i << "is tracked";
          Point2f dps[JointType_Count];

          // left hand filters
          bodies[i].joints[JointType_HandLeft].Position.X = leftMedianX.Process(bodies[i].joints[JointType_HandLeft].Position.X);
          bodies[i].joints[JointType_HandLeft].Position.Y = leftMedianY.Process(bodies[i].joints[JointType_HandLeft].Position.Y);
          bodies[i].joints[JointType_HandLeft].Position.Z = leftMedianZ.Process(bodies[i].joints[JointType_HandLeft].Position.Z);

          bodies[i].joints[JointType_HandLeft].Position.X = leftAverageX.Process(bodies[i].joints[JointType_HandLeft].Position.X);
          bodies[i].joints[JointType_HandLeft].Position.Y = leftAverageY.Process(bodies[i].joints[JointType_HandLeft].Position.Y);
          bodies[i].joints[JointType_HandLeft].Position.Z = leftAverageZ.Process(bodies[i].joints[JointType_HandLeft].Position.Z);

          bodies[i].left = (HandState)((int)leftHandStateMedian.Process(bodies[i].left));

          // right hand filters
          bodies[i].joints[JointType_HandRight].Position.X = rightMedianX.Process(bodies[i].joints[JointType_HandRight].Position.X);
          bodies[i].joints[JointType_HandRight].Position.Y = rightMedianY.Process(bodies[i].joints[JointType_HandRight].Position.Y);
          bodies[i].joints[JointType_HandRight].Position.Z = rightMedianZ.Process(bodies[i].joints[JointType_HandRight].Position.Z);

          bodies[i].joints[JointType_HandRight].Position.X = rightLSEX.Process(bodies[i].joints[JointType_HandRight].Position.X);
          bodies[i].joints[JointType_HandRight].Position.Y = rightLSEY.Process(bodies[i].joints[JointType_HandRight].Position.Y);
          bodies[i].joints[JointType_HandRight].Position.Z = rightLSEZ.Process(bodies[i].joints[JointType_HandRight].Position.Z);

          bodies[i].right = (HandState)((int)rightHandStateMedian.Process(bodies[i].right));

          for(int jointId=0; jointId<JointType_Count; jointId++)
          {
            DepthSpacePoint p;
            sensor.getMapper()->MapCameraPointToDepthSpace(bodies[i].joints[jointId].Position, &p);
            dps[jointId].x = p.X;
            dps[jointId].y = p.Y;
          }

          if(lastRightHandState != HandState_Closed && bodies[i].right == HandState_Closed)
          {
            drawing = true;
            rightTraj.clear();
          }
          if(lastRightHandState == HandState_Closed && bodies[i].right != HandState_Closed)
          {
            drawing = false;
          }

          lastRightHandState = bodies[i].right;

          if(drawing && !drawingTemplate)
          {
            rightTraj.push_back(dps[JointType_HandRight]);
//            qDebug() << "rightTraj.size()=" << rightTraj.size();
            Mat trajMat(512, 424, CV_8UC3, Scalar::all(255));
            for(int i=0; i<rightTraj.size()-1; i++)
            {
//              qDebug() << "drawing line #" << i;
              line(trajMat, rightTraj[i], rightTraj[i+1], Scalar::all(0), 2);
            }
            for(int i=0; i<rightTraj.size(); i++)
            {
//              qDebug() << "drawing point #" << i;
              circle(trajMat, rightTraj[i], 2, Scalar(255, 0, 0), -1);
            }
            emit SendGestureFrame(trajMat);
          }

          infrared = DrawBody(infrared, bodies[i].joints, dps, Scalar(0, 0, 255));
          infrared = DrawHand(infrared, dps[JointType_HandLeft], bodies[i].left, dps[JointType_HandRight], bodies[i].right);
        }
      }
    }

    emit SendDepthFrame(infrared);
  }
}
