#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVector>

class Action
{
public:
  enum ActionType
  {
    ActionType_Unknown = -1,

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

  bool mouseLeftRight;
  int mouseWheelAmount;
  int keyVK;
  QString programPath;
};

class StateTransfer
{
public:
  QString from;
  QString trans;
  QString to;
  Action action;
};

class StateMachine
{
public:
  StateMachine();
  void LoadConfig(QString fileName);
  void Transfer(QString transName);
  void Clear();

private:
  QVector<QString> stateList;
  QVector<StateTransfer> transferList;
  QString nowState;
};

#endif // STATEMACHINE_H
