#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>

class DraggableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DraggableWidget(QWidget *parent = nullptr);
    virtual ~DraggableWidget() = default;

protected:
    bool m_dragging = false;
    bool m_resizing = false;
    QPoint m_dragPosition;
    QPoint m_resizeStartPos;
    QRect m_originalGeometry;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};
