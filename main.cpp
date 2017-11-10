#include "viewmodel.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  qRegisterMetaType<Mat>("Mat");
  QApplication a(argc, argv);
  ViewModel viewModel;
  MainWindow w;

  QObject::connect(&viewModel, &ViewModel::SendDepthFrame, &w, &MainWindow::GetDepthFrame, Qt::QueuedConnection);
  QObject::connect(&viewModel, &ViewModel::SendGestureFrame, &w, &MainWindow::GetGestureFrame, Qt::QueuedConnection);
  QObject::connect(&viewModel, &ViewModel::SendGestureScene, &w, &MainWindow::GetGestureScene, Qt::QueuedConnection);

  QObject::connect(&w, &MainWindow::SigOpenGesture, &viewModel, &ViewModel::GetOpenGestureFileName, Qt::QueuedConnection);
  QObject::connect(&w, &MainWindow::SigCloseGesture, &viewModel, &ViewModel::CloseGesture, Qt::QueuedConnection);
  QObject::connect(&w, &MainWindow::SigDrawGesture, &viewModel, &ViewModel::DrawGesture, Qt::QueuedConnection);
  QObject::connect(&w, &MainWindow::SigLoadConfig, &viewModel, &ViewModel::GetLoadConfigFileName, Qt::QueuedConnection);
  QObject::connect(&w, &MainWindow::SigDrawPoint, &viewModel, &ViewModel::DrawGesturePoint, Qt::QueuedConnection);
  QObject::connect(&w, &MainWindow::SigSaveGesture, &viewModel, &ViewModel::GetSaveGestureFileName, Qt::QueuedConnection);

  w.show();
  viewModel.start();

  int result = a.exec();

  viewModel.quit();
  viewModel.wait();
  return result;
}
