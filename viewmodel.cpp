#include "viewmodel.h"

static void DrawTrack(Mat &input, Points points)
{
  for(int i=0; i<points.size()-1; i++)
  {
//              qDebug() << "drawing line #" << i;
    line(input, PublicTools::QV2D2P2f(points[i]), PublicTools::QV2D2P2f(points[i+1]), Scalar::all(0), 4);
  }
  for(int i=0; i<points.size(); i++)
  {
//              qDebug() << "drawing point #" << i;
    circle(input, PublicTools::QV2D2P2f(points[i]), 8, Scalar(255, 0, 0), -1);
  }
}

ViewModel::ViewModel() :
  stateMachine(),
  dollarOne(),
  leftHandStateMedian(),
  rightHandStateMedian(),
  left(LEFT, &stateMachine, &dollarOne),
  right(RIGHT, &stateMachine, &dollarOne)
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
  emit SigStatus(status);
}

bool ViewModel::GetOpenGestureFileName(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }
  gestureTemplate.clear();
  QTextStream in(&file);
  while(!in.atEnd())
  {
    double x, y;
    in >> x;
    if(in.atEnd()) break;
    in >> y;
    gestureTemplate.push_back(QVector2D(x, y));
  }

  gestureTemplate = DollarOne::Normalize(gestureTemplate, 1.0);
  gestureTemplate = DollarOne::TranslateTo(gestureTemplate, QVector2D(0.5, 0.5));

  QGraphicsScene* scene = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
  PublicTools::DrawPoints(gestureTemplate, scene);
  currentTemplateFileName = fileName;
  emit SendGestureScene(scene);

  status = Status_ShowTemplate;
  emit SigStatus(status);
  return true;
}

void ViewModel::CloseGesture()
{
  status = Status_ShowUserHand;
  emit SigStatus(status);
  gestureTemplate.clear();
}

void ViewModel::DrawGesture()
{
  status = Status_DrawTemplate;
  emit SigStatus(status);
  drawingGesture.clear();
}

void ViewModel::DrawGesturePoint(QVector<QVector2D> point)
{
  if(status == Status_DrawTemplate)
  {
    drawingGesture = point;
//    Mat drawingGestureMat(512, 424, CV_8UC3, Scalar::all(255));
//    DrawTrack(drawingGestureMat, drawingGesture);
//    emit SendGestureFrame(drawingGestureMat);
    QGraphicsScene* scene = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
    PublicTools::DrawPoints(drawingGesture, scene);
    emit SendGestureScene(scene);
  }
}

bool ViewModel::GetSaveGestureFileName(QString fileName)
{
  qDebug() << "GetSaveGestureFileName" << fileName;
  QFile file(fileName);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return false;
  }
  QTextStream out(&file);
//  out << "haha" << endl;
  for(int i=0; i<drawingGesture.size(); i++)
  {
//    qDebug() << "save point #" << i;
    out << drawingGesture[i].x() << " " << drawingGesture[i].y() << endl;
  }
  file.close();
  status = Status_ShowUserHand;
  emit SigStatus(status);
}

bool ViewModel::GetLoadConfigFileName(QString fileName)
{
  usingConfig = true;
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "fileName=" << fileName;
  currentConfigFileName = fileName;
  QFileInfo fileInfo(fileName);
  ErrorInfo result = stateMachine.LoadConfig(fileName);

  if(result.code != ErrorInfo::Error_Success)
  {
    usingConfig = false;
    emit SendLoadConfigError(result);
    return false;
  }

  dollarOne.Clear();
  QString fileFolder = fileInfo.absolutePath();
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "fileFolder=" << fileFolder;
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "stateMachine.transferList.size()=" << stateMachine.transferList.size();
  qDebug() << "ViewModel::GetLoadConfigFileName : " << "stateMachine.stateList.size()=" << stateMachine.stateList.size();
  for(int i=0; i<stateMachine.transferList.size(); i++)
  {
    QString newGestureName = stateMachine.transferList[i].trans;
    qDebug() << "gesture file #" << i << " : " << fileFolder+"/"+newGestureName;
    if(dollarOne.AddTemplate(fileFolder+"/"+newGestureName) == -1)
    {
      result.code = ErrorInfo::Error_NoSuchTemplate;
      result.info = "No such template: "+fileFolder+"/"+newGestureName;
      usingConfig = false;
      stateMachine.Clear();

      emit SendLoadConfigError(result);
      break;
    }
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
//    Mat gestureMat(512, 424, CV_8UC3, Scalar::all(255));
//    DrawTrack(gestureMat, gestureTemplate);
    QGraphicsScene* scene = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
    PublicTools::DrawPoints(gestureTemplate, scene);
    emit SendGestureScene(scene);
    emit SigStatusMsg(cMsgShowTemplate.arg(currentTemplateFileName));
//    emit SendGestureFrame(gestureMat);
  }

  if(status == Status_DrawTemplate)
  {
    emit SigStatusMsg(cMsgDrawTemplate);
  }

  if(status == Status_ShowUserHand)
  {
    if(usingConfig)
    {
      emit SigStatusMsg(cMsgUsingConfig.arg(currentConfigFileName));
    }
    else
    {
      emit SigStatusMsg(cMsgShowUserHand);
    }
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


          stateMachine.HandMove(
                PublicTools::P2f2QV2D(dps[JointType_HandLeft]-dps[JointType_ShoulderLeft]), bodies[i].left,
                PublicTools::P2f2QV2D(dps[JointType_HandRight]-dps[JointType_ShoulderRight]), bodies[i].right);

//          Mat trajMat(512, 424, CV_8UC3, Scalar::all(255));
          QGraphicsScene* scene = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
          left.Process(dps[JointType_HandLeft], bodies[i].left, scene);
          right.Process(dps[JointType_HandRight], bodies[i].right, scene);

          if(status == Status_ShowUserHand)
          {
//              QGraphicsScene* sdddd = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
//              Points ps;
//              ps.push_back(QVector2D(0.0, 0.0));
//              ps.push_back(QVector2D(0.5, 0.5));
//              ps.push_back(QVector2D(1.0, 1.0));
//              PublicTools::DrawPoints(ps, sdddd);
//              sdddd->addRect(0.0, 0.0, 100.0, 100.0);
//            emit SendGestureFrame(trajMat);
            emit SendGestureScene(scene);
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
