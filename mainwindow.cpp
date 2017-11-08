#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  itemDepth = nullptr;
  sceneDepth = nullptr;

  itemGesture = nullptr;
  sceneGesture = nullptr;
  setWindowFlags(Qt::WindowStaysOnTopHint);
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

void MainWindow::GetGestureFrame(Mat gesture)
{
  if(itemGesture!=nullptr)
  {
    delete itemGesture;
    itemGesture = nullptr;
  }
  if(sceneGesture!=nullptr)
  {
    delete sceneGesture;
    sceneGesture = nullptr;
  }
  QImage image(gesture.data, gesture.cols, gesture.rows, gesture.cols*3, QImage::Format_RGB888);
  itemGesture = new QGraphicsPixmapItem(QPixmap::fromImage(image));
  sceneGesture = new QGraphicsScene();
  sceneGesture->addItem(itemGesture);
  ui->gvGesture->setScene(sceneGesture);
  ui->gvGesture->fitInView(ui->gvGesture->scene()->sceneRect(), Qt::KeepAspectRatio);
  ui->gvGesture->show();
}

void MainWindow::on_buttonOpenGesture_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Open a gesture");
  if(fileName!="")
  {
    emit SigOpenGesture(fileName);
  }
}

void MainWindow::on_buttonCloseGesture_clicked()
{
  emit SigCloseGesture();
}

void MainWindow::on_buttonDrawGesture_clicked()
{
  emit SigDrawGesture();
}

void MainWindow::on_buttonSaveGesture_clicked()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Save gesture");
  if(fileName!="")
  {
    emit SigSaveGesture(fileName);
  }
}

void MainWindow::on_buttonLoadConfig_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Open a config file");
  if(fileName!="")
  {
    emit SigLoadConfig(fileName);
  }
}

void MainWindow::on_buttonChangeInput_clicked()
{
  emit SigChangeInput();
}
