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

  w.show();
  viewModel.start();

  int result = a.exec();

  viewModel.quit();
  viewModel.wait();
  return result;
}
