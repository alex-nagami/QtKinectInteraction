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
  QMap<QString, bool> tempCursorMap;
  bool error = false;

  StateTransfer transRule;
  transRule.from = originalState;

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
    else if(cmd == "STATE")
    {
      CHECKSTREAM(in, error);
      in >> transRule.from;
      transRule.from = transRule.from.toUpper();
      if(!CheckMember(tempStateList, transRule.from)) BREAK(error);
    }
    else if(cmd == "CURSOR")
    {
      QString state;
      CHECKSTREAM(in, error);
      in >> state;
      state = state.toUpper();

      if(!CheckMember(tempStateList, state))
      {
        BREAK(error);
      }
      if(cursorMap.find(state)!=cursorMap.end())
      {
        BREAK(error); // cant assign two cursor input source in a single state
      }

      CHECKSTREAM(in, error);
      QString hand;
      in >> hand;
      hand = hand.toUpper();
      bool bh;
      if(hand == "LEFT") bh = LEFT;
      else if(hand == "RIGHT") bh = RIGHT;
      else BREAK(error);

      tempCursorMap.insert(state, bh);
    }
    else if(cmd == "HAND")
    {
      CHECKSTREAM(in, error);
      QString hand;
      in >> hand;
      hand = hand.toUpper();
      if(hand == "LEFT")
      {
        transRule.hand = LEFT;
      }
      else if (hand == "RIGHT")
      {
        transRule.hand = RIGHT;
      }
      else
      {
        BREAK(error);
      }
    }
    else if(cmd == "TEMPLATE")
    {
      QString actionTypeStr;
      CHECKSTREAM(in, error);
      in >> transRule.trans;

      CHECKSTREAM(in, error);
      in >> actionTypeStr;
      actionTypeStr = actionTypeStr.toUpper();

      if(actionTypeStr == "MOUSECURSOR")
      {
        transRule.action.type = Action::ActionType_MouseCursor;
      }
      else if(actionTypeStr == "MOUSE" || actionTypeStr == "MOUSEDOWN" || actionTypeStr == "MOUSEUP")
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
          transRule.action.mouseLeftRight = LEFT;
        }
        else if(key == "RIGHT")
        {
          transRule.action.mouseLeftRight = RIGHT;
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


      if(!error)
      {
        tempTransferList.push_back(transRule);
        emit GetNewTransfer(transRule.trans);
        transRule.from = originalState;
      }
      else
      {
        BREAK(error);
      }
    }
  }

  if(!error)
  {
    stateList = tempStateList;
    transferList = tempTransferList;
    nowState = originalState;
    cursorMap = tempCursorMap;
  }

  return error;
}

void StateMachine::Transfer(QString transName, bool hand)
{
  for(int i=0; i<transferList.size(); i++)
  {
    if(transferList[i].from == nowState)
    {
      if(transferList[i].trans == transName && transferList[i].hand == hand)
      {
        ExecuteAction(transferList[i].action);
        nowState = transferList[i].to;
        break;
      }
    }
  }
}

void StateMachine::ExecuteAction(Action action)
{
  // mouse button events
  if(action.type == Action::ActionType_Key || action.type == Action::ActionType_KeyDown)
  {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wVk = action.keyVK;
    input.ki.time = GetTickCount();
    input.ki.dwFlags = 0; // send key down event;
    SendInput(1, &input, sizeof(input));
  }
  if(action.type == Action::ActionType_KeyUp || action.type == Action::ActionType_Key)
  {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wVk = action.keyVK;
    input.ki.time = GetTickCount();
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
  }
  // mouse button events
  if(action.type == Action::ActionType_MouseDown || action.type == Action::ActionType_Mouse)
  {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.dwExtraInfo = GetMessageExtraInfo();
    input.mi.time = GetTickCount();
    input.mi.mouseData = 0;
    if(action.mouseLeftRight == LEFT)
    {
      input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    }
    else
    {
      input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    }
    SendInput(1, &input, sizeof(input));
  }
  if(action.type == Action::ActionType_MouseUp || action.type == Action::ActionType_Mouse)
  {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.dwExtraInfo = GetMessageExtraInfo();
    input.mi.time = GetTickCount();
    input.mi.mouseData = 0;
    if(action.mouseLeftRight == LEFT)
    {
      input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }
    else
    {
      input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    }
    SendInput(1, &input, sizeof(input));
  }
  // mouse wheel event
  if(action.type == Action::ActionType_MouseWheel)
  {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = action.mouseWheelAmount;
    input.mi.dwExtraInfo = GetMessageExtraInfo();
    input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    SendInput(1, &input, sizeof(input));
  }
  // program event
  if(action.type == Action::ActionType_Program)
  {
    system(action.programPath.toLocal8Bit());
  }
}

void StateMachine::HandMove(QVector2D left, QVector2D right)
{
  if(cursorMap.find(nowState) != cursorMap.end())
  {
    bool bh = cursorMap.find(nowState).value();
    QVector2D source;
    if(bh == LEFT) source = left;
    else source = right;
  }
}
