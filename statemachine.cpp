#include "statemachine.h"

#define CHECKSTREAM(x, y) if(y=x.atEnd())break;
#define BREAK(x) x=true;break;

bool CheckMember(QVector<QString> array, QString e)
{
  for(int i=0; i<array.size(); i++)
  {
    if(array[i] == e) return true;
  }
  return false;
}

StateMachine::StateMachine()
{
}

bool StateMachine::LoadConfig(QString fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return false;
  }

  QTextStream in;
  QVector<QString> tempStateList;
  tempStateList.push_back(originalState);
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
      CHECKSTREAM(in, error);
      in >> newState;
      newState = newState.toUpper();
      if(CheckMember(tempStateList, newState)) BREAK(error);
    }
    else // state transfer rules
    {
      StateTransfer transRule;
      transRule.from = originalState;
      QString actionTypeStr;
      if(cmd == "STATE")
      {
        CHECKSTREAM(in, error);
        in >> transRule.from;
        transRule.from = transRule.from.toUpper();
        if(!CheckMember(tempStateList, transRule.from)) BREAK(error);
      }

      if(cmd == "TEMPLATE")
      {
        CHECKSTREAM(in, error);
        in >> transRule.trans;

        CHECKSTREAM(in, error);
        in >> actionTypeStr;
        actionTypeStr = actionTypeStr.toUpper();
        if(actionTypeStr == "MOUSE" || actionTypeStr == "MOUSEDOWN" || actionTypeStr == "MOUSEUP")
        {
          if(actionTypeStr == "MOUSE") transRule.action.type = Action::ActionType_Mouse;
          if(actionTypeStr == "MOUSEDOWN") transRule.action.type = Action::ActionType_MouseDown;
          if(actionTypeStr == "MOUSEUP") transRule.action.type = Action::ActionType_MouseUp;
          CHECKSTREAM(in, error);
          QString key;
          in >> key;
          key = key.toUpper();
          if(key == "LEFT")
          {
            transRule.action.mouseLeftRight = true;
          }
          else if(key == "RIGHT")
          {
            transRule.action.mouseLeftRight = false;
          }
          else
          {
            BREAK(error);
          }
        }
        else if (actionTypeStr == "MOUSEWHEEL")
        {
          transRule.action.type = Action::ActionType_MouseWheel;
          CHECKSTREAM(in, error);
          in >> transRule.action.mouseWheelAmount;
        }
        else if (actionTypeStr == "KEY" || actionTypeStr == "KEYDOWN" || actionTypeStr == "KEYUP")
        {
          if(actionTypeStr == "KEY") transRule.action.type = Action::ActionType_Key;
          if(actionTypeStr == "KEYDOWN") transRule.action.type = Action::ActionType_KeyDown;
          if(actionTypeStr == "KEYUP") transRule.action.type = Action::ActionType_KeyUp;
          CHECKSTREAM(in, error);
          in >> transRule.action.keyVK;
        }
        else if (actionTypeStr == "PROGRAM")
        {
          transRule.action.type = Action::ActionType_Program;
          QString command;
          CHECKSTREAM(in, error);
          in >> command;
          transRule.action.programPath = command;
          if(command[0] == '"')
          {
            while(command[command.length()-1]!='"')
            {
              CHECKSTREAM(in, error);
              in >> command;
              transRule.action.programPath = transRule.action.programPath+" " + command;
            }
          }
        }
        else
        {
          BREAK(error);
        }

        CHECKSTREAM(in, error);
        in >> transRule.to;
        transRule.to = transRule.to.toUpper();
        if(!CheckMember(tempStateList, transRule.to)) BREAK(error);
      }
      else
      {
        BREAK(error);
      }

      if(!error)
      {
        tempTransferList.push_back(transRule);
        emit GetNewTransfer(transRule.trans);
      }
    }
  }

  if(!error)
  {
    stateList = tempStateList;
    transferList = tempTransferList;
    nowState = originalState;
  }

  return error;
}
