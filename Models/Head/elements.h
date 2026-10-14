#ifndef ELEMENTS_H
#define ELEMENTS_H
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>
class MainWindow;
class Elements  : public QWidget

{
public:
    Elements(MainWindow *mainwindow, QWidget *parent);
    void AddElement();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    void showElement();
    struct Btn {
        QString desc;
        QString iconPath;
        std::function<void()> onClick;
    };
    Btn button[4];
    QPoint m_dragPosition;
    QPoint m_resizeStartPos;
    QRect m_originalGeometry;
    bool m_dragging = false;
    bool m_resizing = false;
    bool m_fixed = false;
    bool m_visible = true;
    QLabel m_label;
    QWidget* m_leftPanel;
    QWidget* m_mainPanel;
    MainWindow* m_mainwindow;
};

#endif // ELEMENTS_H
