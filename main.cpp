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

  QObject::connect(&w, &MainWindow::SigOpenGesture, &viewModel, &ViewModel::GetOpenGestureFileName);
  QObject::connect(&w, &MainWindow::SigCloseGesture, &viewModel, &ViewModel::CloseGesture);
  QObject::connect(&w, &MainWindow::SigLoadConfig, &viewModel, &ViewModel::GetLoadConfigFileName);


  w.show();
  viewModel.start();

  int result = a.exec();

  viewModel.quit();
  viewModel.wait();
  return result;
}
