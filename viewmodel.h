#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QObject>
#include "EasyKinect.h"

class ViewModel : public QObject
{
  Q_OBJECT
public:
  explicit ViewModel(QObject *parent = 0);

signals:
  void ErrorCode(int);
public slots:
  bool Start();

private:
  KinectSensor sensor;
};

#endif // VIEWMODEL_H
