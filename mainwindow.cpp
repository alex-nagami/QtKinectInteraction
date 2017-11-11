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
  setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
  setWindowOpacity(0.8);
  mouseDrawing = false;
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
  drawTemplate = true;
}

void MainWindow::on_buttonSaveGesture_clicked()
{
  if(!drawTemplate) return;
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

void MainWindow::on_buttonExit_clicked()
{
  exit(0);
}

void MainWindow::on_gvGesture_MouseEvent(QMouseEvent *event)
{
  if(mouseDrawing)
  {
    QVector2D pos(event->pos());
    pos.setX(pos.x()/ui->gvGesture->rect().width());
    pos.setY(pos.y()/ui->gvGesture->rect().height());
    drawPoints.push_back(pos);
    qDebug() << "MouseMove: " << pos;
    emit SigDrawPoint(drawPoints);
  }
}

void MainWindow::on_gvGesture_MousePressEvent(QMouseEvent *event)
{
  qDebug() << "MousePressed: " << event->pos();
  if(event->button() == Qt::LeftButton)
  {
    mouseDrawing = true;
    drawPoints.clear();
  }
}

void MainWindow::on_gvGesture_MouseReleaseEvent(QMouseEvent *)
{
  qDebug() << "MouseReleased: " << drawPoints.size();
  mouseDrawing = false;
  drawPoints.clear();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if(movingWindow)
  {
    this->move(this->pos()+event->pos()-mouseOrigin);
  }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
  {
    movingWindow = true;
    mouseOrigin = event->pos();
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
  {
    movingWindow = false;
  }
}

void MainWindow::GetGestureScene(QGraphicsScene *scene)
{
//  qDebug() << "get scene";
  ui->gvGesture->setScene(scene);
  ui->gvGesture->fitInView(ui->gvGesture->sceneRect());
  ui->gvGesture->show();
//  QGraphicsScene* sdddd = new QGraphicsScene(0.0, 0.0, 100.0, 100.0);
//  Points ps;
//  ps.push_back(QVector2D(0.0, 0.0));
//  ps.push_back(QVector2D(0.5, 0.5));
//  ps.push_back(QVector2D(1.0, 1.0));
//  PublicTools::DrawPoints(ps, sdddd);
//  sdddd->addRect(0.0, 0.0, 100.0, 100.0);
//  ui->gvGesture->setScene(sdddd);
//  ui->gvGesture->fitInView(ui->gvGesture->scene()->sceneRect());
//  ui->gvGesture->show();
}

void MainWindow::GetLoadConfigError(ErrorInfo info)
{
  if(info.code!=ErrorInfo::Error_Success)
  {
    QMessageBox::information(this, "Error while loading config", info.info);
  }
}
