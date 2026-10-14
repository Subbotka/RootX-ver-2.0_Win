#ifndef TOOLTABBAR_H
#define TOOLTABBAR_H

#include <QWidget>
#include "Models/Head/mainwindow.h"
#include "Models/Head/basemenu.h"
namespace Ui {
class ToolTabBar;
}
class MainWindow;
class ToolTabBar : public QWidget
{
    Q_OBJECT

public:
    explicit ToolTabBar(MainWindow *mainwindow = nullptr, QWidget *parent = nullptr);
    ~ToolTabBar();

private slots:
    void on_tabWidget_tabBarClicked(int index);

private:
    Ui::ToolTabBar *ui;
    MainWindow *m_mainwindow;
    BaseMenu *m_menu;
};

#endif // TOOLTABBAR_H
