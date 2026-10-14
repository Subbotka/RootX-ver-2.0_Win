#include "Models/Head/elements.h"
#include "Models/Head/mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>

Elements::Elements( MainWindow *mainwindow, QWidget *parent)
    : QWidget(parent)
    , m_mainwindow(mainwindow)

{
    button[0] = {"Добавить текст", ":/new/prefix1/app_thing/text.png", [this](){ AddElement(); }};
    button[1] = {"Добавить фигуру", ":/new/prefix1/app_thing/shapes.png", [this](){ AddElement(); }};
    button[2] = {"Скрыть панель", ":/new/prefix1/app_thing/view.png", [this](){ showElement(); }};
    button[3] = {"Зафиксировать", ":/new/prefix1/app_thing/lock.png", [this](){ m_fixed = m_fixed? false:true; }};

}



void Elements::AddElement() {

    this->setParent(m_mainwindow->ui->imagelabel);
    this->setGeometry(10, 10, 100, 100);
    this->setStyleSheet("background-color: white;");
    this->show();
    this->raise();

    QHBoxLayout *Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(0, 0, 0, 0);
    Hlayout->setSpacing(5);

    m_leftPanel = new QWidget();
    m_leftPanel->setStyleSheet("background-color: rgb(222, 221, 218);");

    m_mainPanel = new QWidget();
    m_mainPanel->setStyleSheet("background-color: white;");
    Hlayout->addWidget(m_leftPanel);
    Hlayout->addWidget(m_mainPanel, 1);

    QVBoxLayout *Vlayout = new QVBoxLayout(m_leftPanel);
    Vlayout->setSpacing(3);
    Vlayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 4; i++) {

        QIcon icon(button[i].iconPath);

        if (icon.isNull()) {
            qDebug() << "❌ Не удалось загрузить иконку по пути:" << button[i].iconPath;
        }

        QPushButton* btn = new QPushButton(icon, "");
        btn->setFixedSize(32, 32);
        btn->setIconSize(QSize(24, 24));
        btn->setToolTip(button[i].desc);

        Vlayout->addWidget(btn);

        connect(btn, &QPushButton::clicked, button[i].onClick);
    }


    Vlayout->addStretch(1);

    this->show();
    this->raise();

}
void Elements::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (!m_visible) {
            this->setStyleSheet("background-color: white;");
            m_mainPanel->setStyleSheet("background-color: white;");
            m_leftPanel->show();
            m_visible = true;
        }
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (event->pos().x() > width() - 20 || event->pos().y() > height() - 20) {
            m_resizing = true;
            m_resizeStartPos = event->globalPosition().toPoint();
            m_originalGeometry = geometry();
        } else {
            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        }
        event->accept();
        raise();

    }
}

void Elements::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resizeStartPos;
        QRect newGeo = m_originalGeometry;
        newGeo.setRight(newGeo.right() + delta.x());
        newGeo.setBottom(newGeo.bottom() + delta.y());

        if (newGeo.width() < 100) newGeo.setRight(newGeo.left() + 100);
        if (newGeo.height() < 60) newGeo.setBottom(newGeo.top() + 60);

        QRect bounds = m_mainwindow->ui->imagelabel->geometry();
        if (newGeo.right() > bounds.right()) newGeo.setRight(bounds.right());
        if (newGeo.bottom() > bounds.bottom()) newGeo.setBottom(bounds.bottom());

        setGeometry(newGeo);
        event->accept();

    } else if (m_dragging) {
        QPoint newPos = event->globalPosition().toPoint() - m_dragPosition;

        QRect bounds = m_mainwindow->ui->imagelabel->geometry();
        QRect widgetRect = geometry();

        if (newPos.x() < bounds.left()) newPos.setX(bounds.left());
        if (newPos.y() < bounds.top()) newPos.setY(bounds.top());
        if (newPos.x() + widgetRect.width() > bounds.right())
            newPos.setX(bounds.right() - widgetRect.width());
        if (newPos.y() + widgetRect.height() > bounds.bottom())
            newPos.setY(bounds.bottom() - widgetRect.height());

        if (m_fixed) return;
        move(newPos);
        event->accept();

    } else {
        if (event->pos().x() > width() - 20 || event->pos().y() > height() - 20)
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::ArrowCursor);
    }
}

void Elements::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_resizing = false;
        event->accept();
    }
}

void Elements::showElement() {
    this->setStyleSheet("border: none;");
    m_mainPanel->setStyleSheet("border: none;");
    m_leftPanel->hide();
    m_visible = false;
}
