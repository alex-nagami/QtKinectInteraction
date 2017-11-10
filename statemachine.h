#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QDir>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QVector2D>
#include <Windows.h>
#include <WinUser.h>
#include "EasyKinect.h"

#define LEFT (true)
#define RIGHT (false)

struct ErrorInfo
{
  enum ErrorCode{
    Error_Success = 0,
    Error_InvalidToken,
    Error_MissingToken
  };

  ErrorCode code;
  QString info;
  QString relatedItem;
  int lineNum;
};

class Action
{
public:
  enum ActionType
  {
    ActionType_Unknown = -1,

    ActionType_MouseCursor,

    ActionType_Mouse,
    ActionType_MouseDown,
    ActionType_MouseUp,

    ActionType_MouseWheel,

    ActionType_Key,
    ActionType_KeyDown,
    ActionType_KeyUp,

    ActionType_Program
  };

  ActionType type;

  bool mouseLeftRight; // true=left, false=right
  int mouseWheelAmount;
  int keyVK;
  QString programPath;
};

class StateTransfer
{
public:
  bool hand; // true=left, false=right;
  QString from;
  QString trans;
  QString to;
  Action action;
};

class StateMachine : QObject
{
  Q_OBJECT
signals:
  void GetNewTransfer(QString fileName);

public:

  const QString originalState = "ORIGIN";

  StateMachine();
  bool LoadConfig(QString fileName);
  void Transfer(QString transName, bool hand);
  void Clear();
  void ExecuteAction(Action action);
  void HandMove(QVector2D left, HandState lhs, QVector2D right, HandState rhs);

  QVector<QString> stateList;
  QVector<StateTransfer> transferList;
  QString nowState;
  QMap<QString, bool> cursorMap;
};

#endif // STATEMACHINE_H
