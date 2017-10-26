#include "statemachine.h"

StateMachine::StateMachine()
{

}

void StateMachine::LoadConfig(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return;
  }

  QTextStream in;
  QVector<QString> tempStateList;
  QVector<StateTransfer> tempTransferList;
  bool error = false;

  while(!in.atEnd())
  {
    QString cmd;
    in >> cmd;
    cmd = cmd.toUpper();

    if(cmd == "#")  // comments
    {
      in.readLine();
      continue;
    }
    else if(cmd == "ADD") // state declarations
    {
      QString newState;
      if(in.atEnd())
      {
        error = true;
        break;
      }
      in >> newState;
      for(int i=0; i<tempStateList.size(); i++)
      {
        if(newState == tempStateList[i])
        {
          error = true;
          break;
        }
      }
      if(error) break;
    }
  }
}
