#ifndef BASEMENU_H
#define BASEMENU_H
#include <QWidget>
#include "mainwindow.h"
namespace Ui {
class BaseMenu;
class MainWindow;
}
class BaseMenu : public QWidget
{
    Q_OBJECT
public:
    explicit BaseMenu(QWidget *parent = nullptr, MainWindow *mainwindow = nullptr);
    ~BaseMenu();

private:
    Ui::BaseMenu *ui;
    MainWindow *m_mainwindow;
};

#endif // BASEMENU_H
