#include "viewmodel.h"

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

static void DrawTrack(Mat &input, QVector<Point2f> points)
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

static void DrawTrack(Mat &input, Points points)
{
  for(int i=0; i<points.size()-1; i++)
  {
//              qDebug() << "drawing line #" << i;
    line(input, QV2D2P2f(points[i]), QV2D2P2f(points[i+1]), Scalar::all(0), 2);
  }
  for(int i=0; i<points.size(); i++)
  {
//              qDebug() << "drawing point #" << i;
    circle(input, QV2D2P2f(points[i]), 2, Scalar(255, 0, 0), -1);
  }
}

ViewModel::ViewModel() :
  leftHandStateMedian(),
  rightHandStateMedian()
{
  HRESULT result;
  result = sensor.init((FrameSourceTypes)(FrameSourceTypes_Depth | FrameSourceTypes_Infrared | FrameSourceTypes_Body));
  if(FAILED(result))
  {
    qDebug() << "Failed to load sensor";
    inited = false;
    status = Status_Error;
    return;
  }
  inited = true;
  status = Status_ShowUserHand;
}

bool ViewModel::GetOpenGestureFileName(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }
  QTextStream in(&file);
  while(!in.atEnd())
  {
    double x, y;
    in >> x >> y;
    gestureTemplate.push_back(QVector2D(x, y));
  }

  dollarOne.AddTemplate(gestureTemplate, QFileInfo(fileName).fileName());

  gestureTemplate = DollarOne::Normalize(gestureTemplate, 300);
  gestureTemplate = DollarOne::TranslateTo(gestureTemplate, QVector2D(256, 212));
  gestureTemplate = DollarOne::Resample(gestureTemplate, dollarOne.pointNum);

  status = Status_ShowTemplate;
  return true;
}

void ViewModel::CloseGesture()
{
  status = Status_ShowUserHand;
  gestureTemplate.clear();
}

void ViewModel::DrawGesture()
{
  status = Status_DrawTemplate;
  drawingGesture.clear();
}

void ViewModel::DrawGesturePoint(QVector2D point)
{
  drawingGesture.push_back(point);
  Mat drawingGestureMat(512, 424, CV_8UC3, Scalar::all(255));
  DrawTrack(drawingGestureMat, drawingGesture);
  emit SendGestureFrame(drawingGestureMat);
}

bool ViewModel::GetSaveGestureFileName(QString fileName)
{
  QFile file(fileName);
  if(file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return false;
  }
  QTextStream out(&file);
  for(int i=0; i<drawingGesture.size(); i++)
  {
    out << drawingGesture[i].x() << " " << drawingGesture[i].y() << endl;
  }
  file.close();
}

bool ViewModel::GetLoadConfigFileName(QString fileName)
{
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "fileName=" << fileName;
  QFileInfo fileInfo(fileName);
  bool result = stateMachine.LoadConfig(fileName);

  if(result) return result;

  dollarOne.Clear();
  QString fileFolder = fileInfo.absolutePath();
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "fileFolder=" << fileFolder;
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "stateMachien.transferList.size()=" << stateMachine.transferList.size();
  for(int i=0; i<stateMachine.transferList.size(); i++)
  {
    QString newGestureName = stateMachine.transferList[i].trans;
    qDebug() << "gesture file #" << i << " : " << fileFolder+"/"+newGestureName;
    dollarOne.AddTemplate(fileFolder+"/"+newGestureName);
  }
  return true;
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
  if(status == Status_ShowTemplate)
  {
    Mat gestureMat(512, 424, CV_8UC3, Scalar::all(255));
    DrawTrack(gestureMat, gestureTemplate);
    emit SendGestureFrame(gestureMat);
  }

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

          if(status == Status_ShowUserHand)
          {
            Mat trajMat(512, 424, CV_8UC3, Scalar::all(255));
            DrawTrack(trajMat, rightTraj);
            emit SendGestureFrame(trajMat);
          }

          infrared = DrawBody(infrared, bodies[i].joints, dps, Scalar(0, 0, 255));
          infrared = DrawHand(infrared, dps[JointType_HandLeft], bodies[i].left, dps[JointType_HandRight], bodies[i].right);

          break;
        }
      }
    }
    emit SendDepthFrame(infrared);
  }
}
