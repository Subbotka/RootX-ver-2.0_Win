#include "Models/Head/imagecontroller.h"
#include <QAction>
#include <QMouseEvent>
#include <QDebug>
#include <QWidget>

ImageController::ImageController(QObject *parent)
    : QObject(parent)
    , m_imageWidget(nullptr)
    , m_handAction(nullptr)
    , m_isDragging(false)
    , m_handCursor(Qt::OpenHandCursor)
    , m_grabCursor(Qt::ClosedHandCursor)
{
}

void ImageController::setupHandAction(QAction *handAction, QWidget *imageWidget)
{
    m_handAction = handAction;
    m_imageWidget = imageWidget;

    m_handAction->setCheckable(true);
    m_handAction->setMenu(nullptr);

    connect(m_handAction, &QAction::toggled,
            this, &ImageController::onHandActionToggled);
}

void ImageController::onHandActionToggled(bool checked)
{
    if (!m_imageWidget) return;

    if (checked) {
        m_imageWidget->installEventFilter(this);//start
        m_imageWidget->setCursor(m_handCursor);
    } else {
        m_imageWidget->removeEventFilter(this);//stop
        m_imageWidget->setCursor(Qt::ArrowCursor);
        stopDragging();
    }
}

bool ImageController::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_imageWidget && m_handAction && m_handAction->isChecked()) {

        switch (event->type()) {

        case QEvent::MouseButtonPress: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                startDragging(mouseEvent->globalPos());
                return true;
            }
            break;
        }

        case QEvent::MouseMove: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (m_isDragging && (mouseEvent->buttons() & Qt::LeftButton)) {
                doDragging(mouseEvent->globalPos());
                return true;
            }
            break;
        }

        case QEvent::MouseButtonRelease: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && m_isDragging) {
                stopDragging();
                return true;
            }
            break;
        }

        default:
            break;
        }
    }

    return QObject::eventFilter(obj, event);
}

void ImageController::startDragging(const QPoint &pos)
{
    if (!m_imageWidget) return;

    m_isDragging = true;
    m_dragStartPosition = pos;
    m_labelStartPosition = m_imageWidget->pos();
    m_imageWidget->setCursor(m_grabCursor);
}

void ImageController::doDragging(const QPoint &pos)
{
    if (!m_isDragging || !m_imageWidget) return;

    QPoint delta = pos - m_dragStartPosition;
    QPoint newPos = m_labelStartPosition + delta;

    QWidget *parent = m_imageWidget->parentWidget();
    if (parent) {
        int maxX = parent->width() - m_imageWidget->width();
        int maxY = parent->height() - m_imageWidget->height();

        newPos.setX(qMax(0, qMin(newPos.x(), maxX)));
        newPos.setY(qMax(0, qMin(newPos.y(), maxY)));
    }

    m_imageWidget->move(newPos);
}

void ImageController::stopDragging()
{
    m_isDragging = false;

    if (m_imageWidget && m_handAction && m_handAction->isChecked()) {
        m_imageWidget->setCursor(m_handCursor);
    }
}
