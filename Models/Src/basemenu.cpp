#include "Models/Head/basemenu.h"
#include "ui_basemenu.h"
#include "ui_mainwindow.h"
BaseMenu::BaseMenu(QWidget *parent, MainWindow *mainwindow)
    : QWidget(parent)
    , ui(new Ui::BaseMenu)
    , m_mainwindow(mainwindow)
{
    ui->setupUi(this);
    //if (m_mainwindow) m_mainwindow->ui->toolBar->hide();
}



BaseMenu::~BaseMenu()
{
    delete ui;
}
