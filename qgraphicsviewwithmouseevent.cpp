#include "qgraphicsviewwithmouseevent.h"

QGraphicsViewWithMouseEvent::QGraphicsViewWithMouseEvent(QWidget* parent) :
  QGraphicsView(parent)
{
}

void QGraphicsViewWithMouseEvent::mousePressEvent(QMouseEvent *event)
{
  emit MouseEvent(event);
  emit MousePressEvent(event);
}

void QGraphicsViewWithMouseEvent::mouseMoveEvent(QMouseEvent *event)
{
  emit MouseEvent(event);
  emit MouseMoveEvent(event);
}

void QGraphicsViewWithMouseEvent::mouseReleaseEvent(QMouseEvent *event)
{
  emit MouseEvent(event);
  emit MouseReleaseEvent(event);
}
