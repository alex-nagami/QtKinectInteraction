#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <WinUser.h>
#include <Windows.h>

#define LEFT (true)
#define RIGHT (false)

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
  const QString originalState = "ORIGINAL";

  StateMachine();
  bool LoadConfig(QString fileName);
  void Transfer(QString transName, bool hand);
  void Clear();
  void ExecuteAction(Action action);
  void HandMove(QVector2D left, QVector2D right);

  QVector<QString> stateList;
  QVector<StateTransfer> transferList;
  QString nowState;
  QMap<QString, bool> cursorMap;
};

#endif // STATEMACHINE_H
