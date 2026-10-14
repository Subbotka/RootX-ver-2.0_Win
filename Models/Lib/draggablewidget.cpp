#include "draggablewidget.h"

DraggableWidget::DraggableWidget(QWidget *parent)
    : QWidget{parent}
{

}


void DraggableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->pos().x() > width() - 20 || event->pos().y() > height() - 20) {
            m_resizing = true;
            m_resizeStartPos = event->globalPosition().toPoint();
            m_originalGeometry = geometry();
        } else {
            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - pos();
        }
        event->accept();
    }
}

void DraggableWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resizeStartPos;
        QRect newGeo = m_originalGeometry;
        newGeo.setRight(newGeo.right() + delta.x());
        newGeo.setBottom(newGeo.bottom() + delta.y());

        if (newGeo.width() < 100) newGeo.setWidth(100);
        if (newGeo.height() < 60) newGeo.setHeight(60);

        QWidget *parent = parentWidget();
        if (parent) {
            if (newGeo.right() > parent->width()) newGeo.setRight(parent->width());
            if (newGeo.bottom() > parent->height()) newGeo.setBottom(parent->height());
        }

        setGeometry(newGeo);
        event->accept();

    } else if (m_dragging) {
        QPoint globalPos = event->globalPosition().toPoint();
        QPoint newPos = globalPos - m_dragPosition;

        QWidget *parent = parentWidget();
        if (parent) {
            if (newPos.x() < 0) newPos.setX(0);
            if (newPos.y() < 0) newPos.setY(0);
            if (newPos.x() + width() > parent->width())
                newPos.setX(parent->width() - width());
            if (newPos.y() + height() > parent->height())
                newPos.setY(parent->height() - height());
        }

        move(newPos);
        event->accept();

    } else {
        if (event->pos().x() > width() - 20 || event->pos().y() > height() - 20)
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::ArrowCursor);
    }
}


void DraggableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_resizing = false;
        event->accept();
    }
}
