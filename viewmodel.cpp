#include "viewmodel.h"

ViewModel::ViewModel(QObject *parent) : QObject(parent)
{

}

bool ViewModel::Start()
{
  HRESULT result;
  result = sensor.init((FrameSourceTypes)(FrameSourceTypes_Depth | FrameSourceTypes_Infrared | FrameSourceTypes_Body));
  if(FAILED(result))
  {
    return false;
  }
}
