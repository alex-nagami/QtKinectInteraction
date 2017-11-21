#include "statemachine.h"

bool CheckMember(QVector<QString> array, QString e)
{
  for(int i=0; i<array.size(); i++)
  {
//    qDebug() << "array[i]=" << array[i];
    if(array[i] == e) return true;
  }
  return false;
}

StateMachine::StateMachine()
{
}

ErrorInfo StateMachine::LoadConfig(QString fileName)
{
  ErrorInfo result;
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    result.code = ErrorInfo::Error_NoSuchFile;
    result.info = "File "+fileName+" doesn't exist or can't be opened";
    return result;
  }

  QTextStream in(&file);
  QVector<QString> tempStateList;
  tempStateList.push_back(originalState);
  QVector<StateTransfer> tempTransferList;
  QMap<QString, bool> tempCursorMap;
  bool error = false;

  StateTransfer transRule;
  transRule.from = originalState;
  transRule.hand = RIGHT;

  while(!in.atEnd())
  {
    QString cmd;
    in >> cmd;
    cmd = cmd.toUpper();
    qDebug() << "StateMachine::LoadConfig" << " : " << "cmd=" << cmd;

    if(cmd == "#")  // comments
    {
      in.readLine();
      continue;
    }
    else if(cmd == "ADD") // state declarations
    {
      QString newState;
      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after ADD";
        break;
      }
      in >> newState;
      newState = newState.toUpper();
//      qDebug() << "CheckMember(tempStateList, newState)=" << CheckMember(tempStateList, newState);
      if(CheckMember(tempStateList, newState))
      {
        result.code = ErrorInfo::Error_InvalidStateName;
        result.info = "State name "+newState+" has already been added";
        error=true;
//        qDebug() << "breaked";
        break;
      }
      tempStateList.push_back(newState);
//      qDebug() << "tempStateList.size()=" << tempStateList.size();
    }
    else if(cmd == "STATE")
    {
      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after STATE";
        break;
      }
      in >> transRule.from;
      transRule.from = transRule.from.toUpper();
      if(!CheckMember(tempStateList, transRule.from))
      {
        result.code = ErrorInfo::Error_InvalidStateName;
        result.info = "State name "+transRule.from+" should be added before being referenced";
        error=true;
        break;
      }
    }
    else if(cmd == "CURSOR")
    {
      QString state;
      if(error = in.atEnd()) break;
      in >> state;
      state = state.toUpper();

      if(!CheckMember(tempStateList, state))
      {
        result.code = ErrorInfo::Error_InvalidStateName;
        result.info = "State name "+state+" should be added before being referenced";
        error=true;
        break;
      }
      if(cursorMap.find(state)!=cursorMap.end())
      {
        result.code = ErrorInfo::Error_ConflictCursor;
        result.info = "State "+state+" can only be assigned ONE cursor pointer";
        error=true;
        break;
         // cant assign two cursor input source in a single state
      }

      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after CURSOR";
        break;
      }
      QString hand;
      in >> hand;
      hand = hand.toUpper();
      bool bh;
      if(hand == "LEFT") bh = LEFT;
      else if(hand == "RIGHT") bh = RIGHT;
      else
      {
        result.code = ErrorInfo::Error_InvalidToken;
        result.info = "Invalid token "+hand+" after CURSOR, only LEFT or RIGHT is accepted";
        error=true;
        break;
      }

      tempCursorMap.insert(state, bh);
    }
    else if(cmd == "HAND")
    {
      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after HAND";
        break;
      }
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
        result.code = ErrorInfo::Error_InvalidToken;
        result.info = "Invalid token "+hand+" after HAND, only LEFT or RIGHT is accpcepted";
        error=true;
        break;
      }
    }
    else if(cmd == "TEMPLATE")
    {
      QString actionTypeStr;
      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after TEMPLATE";
        break;
      }
      in >> transRule.trans;
      transRule.trans = transRule.trans.toUpper();
      qDebug() << "trans=" << transRule.trans;

      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after TEMPLATE";
        break;
      }
      in >> actionTypeStr;
      actionTypeStr = actionTypeStr.toUpper();
      qDebug() << "actionTypeStr=" << actionTypeStr;

      if(actionTypeStr == "MOUSECURSOR")
      {
        transRule.action.type = Action::ActionType_MouseCursor;
      }
      else if(actionTypeStr == "MOUSE" || actionTypeStr == "MOUSEDOWN" || actionTypeStr == "MOUSEUP")
      {
        if(actionTypeStr == "MOUSE") transRule.action.type = Action::ActionType_Mouse;
        if(actionTypeStr == "MOUSEDOWN") transRule.action.type = Action::ActionType_MouseDown;
        if(actionTypeStr == "MOUSEUP") transRule.action.type = Action::ActionType_MouseUp;
        if(error = in.atEnd())
        {
          result.code = ErrorInfo::Error_UnexpectedEOF;
          result.info = "Unexpected EOF after "+actionTypeStr;
          break;
        }
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
          result.code = ErrorInfo::Error_InvalidToken;
          result.info = "Invalid token "+key+" after "+actionTypeStr+", only LEFT or RIGHT is accpcepted";
          error=true;
          break;
        }
      }
      else if (actionTypeStr == "MOUSEWHEEL")
      {
        transRule.action.type = Action::ActionType_MouseWheel;
        if(error = in.atEnd())
        {
          result.code = ErrorInfo::Error_UnexpectedEOF;
          result.info = "Unexpected EOF after MOUSEWHEEL";
          break;
        }
        in >> transRule.action.mouseWheelAmount;
      }
      else if (actionTypeStr == "KEY" || actionTypeStr == "KEYDOWN" || actionTypeStr == "KEYUP")
      {
        if(actionTypeStr == "KEY") transRule.action.type = Action::ActionType_Key;
        if(actionTypeStr == "KEYDOWN") transRule.action.type = Action::ActionType_KeyDown;
        if(actionTypeStr == "KEYUP") transRule.action.type = Action::ActionType_KeyUp;
        if(error = in.atEnd())
        {
          result.code = ErrorInfo::Error_UnexpectedEOF;
          result.info = "Unexpected EOF after "+actionTypeStr;
          break;
        }
        in >> transRule.action.keyVK;
      }
      else if (actionTypeStr == "PROGRAM")
      {
        transRule.action.type = Action::ActionType_Program;
        QString command;
        if(error = in.atEnd())
        {
          result.code = ErrorInfo::Error_UnexpectedEOF;
          result.info = "Unexpected EOF after "+actionTypeStr;
          break;
        }
        in >> command;
        transRule.action.programPath = command;
        if(command[0] == '"')
        {
          while(command[command.length()-1]!='"')
          {
            if(in.atEnd()) break;
            in >> command;
            transRule.action.programPath = transRule.action.programPath+" " + command;
          }
        }
      }
      else
      {
        result.code = ErrorInfo::Error_InvalidToken;
        result.info = "Invalid action type "+actionTypeStr;
        error=true;
        break;
      }

      if(error = in.atEnd())
      {
        result.code = ErrorInfo::Error_UnexpectedEOF;
        result.info = "Unexpected EOF after action";
        break;
      }
      in >> transRule.to;
      transRule.to = transRule.to.toUpper();
      if(!CheckMember(tempStateList, transRule.to))
      {
        result.code = ErrorInfo::Error_InvalidStateName;
        result.info = "State name "+transRule.to+" should be added before being referenced";
        error=true;
        break;
      }


      for(int i=; i<tempTransferList.size(); i++)
      {
        if(tempTransferList[i].from == transRule.from && tempTransferList[i].trans == transRule.trans)
        {
          if(tempTransferList[i].to != transRule.to)
          {
            result.code = ErrorInfo::Error_MultipleTarget;
            result.info = "Transition from "+transRule.from+" via "+transRule.trans+" has multiple targets.";
            error = true;
            break;
          }
          else
          {
          }
        }
      }

      if(!error)
      {
        tempTransferList.push_back(transRule);
        emit GetNewTransfer(transRule.trans);
        transRule.from = originalState;
      }
      else
      {
        error=true;
        break;
      }
    }
    else
    {
      result.code = ErrorInfo::Error_InvalidToken;
      result.info = "Invalid token "+cmd;
      error=true;
      break;
    }
  }

  if(!error)
  {
    stateList = tempStateList;
    transferList = tempTransferList;
    nowState = originalState;
    cursorMap = tempCursorMap;
  }

  return result;
}

void StateMachine::Transfer(QString transName, bool hand)
{
  transName = transName.toUpper();
  qDebug() << "Transfer :" << nowState << transName << hand;
  for(int i=0; i<transferList.size(); i++)
  {
    qDebug() << i << " : " << transferList[i].from << transferList[i].to << transferList[i].trans << transferList[i].hand;
    if(transferList[i].from == nowState)
    {
      if(transferList[i].trans == transName && transferList[i].hand == hand)
      {
        qDebug() << "action" << transName;
        ExecuteAction(transferList[i].action);
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

void StateMachine::HandMove(QVector2D left, HandState lhs, QVector2D right, HandState rhs)
{
  const double xratio = 1.0e-1;
  const double yratio = 1.0e-1;

  static HandState lastState = HandState_Unknown;
  if(cursorMap.find(nowState) != cursorMap.end())
  {
    bool bh = cursorMap.find(nowState).value();
    QVector2D source;
    HandState hsSource;
    if(bh == LEFT)
    {
      source = left;
      hsSource = lhs;
    }
    else
    {
      source = right;
      hsSource = rhs;
    }

    INPUT mouseMove;
    mouseMove.type = INPUT_MOUSE;
    mouseMove.mi.dx = source.x()*xratio;
    mouseMove.mi.dy = source.y()*yratio;
    mouseMove.mi.time = GetTickCount();
    mouseMove.mi.dwExtraInfo = GetMessageExtraInfo();
    mouseMove.mi.dwFlags = MOUSEEVENTF_MOVE;
    qDebug() << "HandMove: " << source;
    SendInput(1, &mouseMove, sizeof(mouseMove));

    if(hsSource == HandState_Unknown || hsSource == HandState_NotTracked)
    {
      hsSource = lastState;
    }

    if(hsSource != HandState_Unknown && hsSource != HandState_NotTracked)
    {
      // left key = closed
      if(lastState == HandState_Open && hsSource == HandState_Closed)
      {
        INPUT mouseMove;
        mouseMove.type = INPUT_MOUSE;
        mouseMove.mi.dx = 0;
        mouseMove.mi.dy = 0;
        mouseMove.mi.mouseData = 0;
        mouseMove.mi.dwExtraInfo = GetMessageExtraInfo();
        mouseMove.mi.time = GetTickCount();
        mouseMove.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &mouseMove, sizeof(mouseMove));
      }
      if(lastState == HandState_Closed && hsSource == HandState_Open)
      {
        INPUT mouseMove;
        mouseMove.type = INPUT_MOUSE;
        mouseMove.mi.dx = 0;
        mouseMove.mi.dy = 0;
        mouseMove.mi.mouseData = 0;
        mouseMove.mi.dwExtraInfo = GetMessageExtraInfo();
        mouseMove.mi.time = GetTickCount();
        mouseMove.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &mouseMove, sizeof(mouseMove));
      }

      // right key = lasso
      if(lastState == HandState_Open && hsSource == HandState_Lasso)
      {
        INPUT mouseMove;
        mouseMove.type = INPUT_MOUSE;
        mouseMove.mi.dx = 0;
        mouseMove.mi.dy = 0;
        mouseMove.mi.mouseData = 0;
        mouseMove.mi.dwExtraInfo = GetMessageExtraInfo();
        mouseMove.mi.time = GetTickCount();
        mouseMove.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        SendInput(1, &mouseMove, sizeof(mouseMove));
      }
      if(lastState == HandState_Lasso && hsSource == HandState_Open)
      {
        INPUT mouseMove;
        mouseMove.type = INPUT_MOUSE;
        mouseMove.mi.dx = 0;
        mouseMove.mi.dy = 0;
        mouseMove.mi.mouseData = 0;
        mouseMove.mi.dwExtraInfo = GetMessageExtraInfo();
        mouseMove.mi.time = GetTickCount();
        mouseMove.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        SendInput(1, &mouseMove, sizeof(mouseMove));
      }

      lastState = hsSource;
    }
  }
}
