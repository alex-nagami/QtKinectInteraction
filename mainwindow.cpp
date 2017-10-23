#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  itemDepth = nullptr;
  sceneDepth = nullptr;
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::GetDepthFrame(Mat depth)
{
  if(itemDepth!=nullptr)
  {
    delete itemDepth;
    itemDepth = nullptr;
  }
  if(sceneDepth!=nullptr)
  {
    delete sceneDepth;
    sceneDepth = nullptr;
  }
  QImage image(depth.data, depth.cols, depth.rows, depth.cols*3, QImage::Format_RGB888);
  itemDepth = new QGraphicsPixmapItem(QPixmap::fromImage(image));
  sceneDepth = new QGraphicsScene();
  sceneDepth->addItem(itemDepth);
  ui->gvDepth->setScene(sceneDepth);
  ui->gvDepth->fitInView(ui->gvDepth->scene()->sceneRect(), Qt::KeepAspectRatio);
  ui->gvDepth->show();
}
