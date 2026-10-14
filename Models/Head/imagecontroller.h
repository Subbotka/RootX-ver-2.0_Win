#ifndef IMAGECONTROLLER_H
#define IMAGECONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QCursor>

class QAction;
class QWidget;

class ImageController : public QObject
{
    Q_OBJECT

public:
    explicit ImageController(QObject *parent = nullptr);

    void setupHandAction(QAction *handAction, QWidget *imageWidget);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onHandActionToggled(bool checked);

private:
    void startDragging(const QPoint &pos);
    void doDragging(const QPoint &pos);
    void stopDragging();

    QWidget *m_imageWidget;
    QAction *m_handAction;

    bool m_isDragging;
    QPoint m_dragStartPosition;
    QPoint m_labelStartPosition;

    QCursor m_handCursor;
    QCursor m_grabCursor;
};

#endif // IMAGECONTROLLER_H
