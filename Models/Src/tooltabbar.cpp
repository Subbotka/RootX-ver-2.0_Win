#include "Models/Head/tooltabbar.h"
#include "Models/Head/basemenu.h"
#include "ui_mainwindow.h"
#include "ui_tooltabbar.h"
#include <QTabBar>
#include <QPropertyAnimation>
ToolTabBar::ToolTabBar(MainWindow *mainwindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToolTabBar)
    , m_mainwindow(mainwindow)
    , m_menu(nullptr)
{
    ui->setupUi(this);


}

ToolTabBar::~ToolTabBar()
{
    delete ui;
}

bool GetVisible(QTabWidget* tab) {
    return tab->isVisible();
}


void ToolTabBar::on_tabWidget_tabBarClicked(int index)
{

    if (index == 0) {
        if (m_mainwindow->ui->mainborder->isVisible()) {
            m_mainwindow->ui->mainborder->hide();

            if (m_menu) {
                delete m_menu;
                m_menu = nullptr;
            }

            m_menu = new BaseMenu(m_mainwindow);
            ui->horizontalLayout_2->addWidget(m_menu);
            QPoint startPos(-m_menu->width(), 0);
            QPoint endPos(0, 0);

            m_menu->move(startPos);
            m_menu->raise();
            m_menu->show();

            QPropertyAnimation *animation = new QPropertyAnimation(m_menu, "pos");
            animation->setDuration(150);
            animation->setStartValue(startPos);
            animation->setEndValue(endPos);
            animation->setEasingCurve(QEasingCurve::OutBack);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        }

    }

    else {
        if (!m_mainwindow->ui->mainborder->isVisible()) {
            m_mainwindow->ui->mainborder->show();
            if (m_menu) m_menu->close();
        }
    }


}
