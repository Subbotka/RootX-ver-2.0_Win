#ifndef LOADING_H
#define LOADING_H
#include <QWidget>
#include "mainwindow.h"
namespace Ui {
class Loading;
class MainWindow;
}

class Loading : public QWidget
{
    Q_OBJECT

public:
    explicit Loading(MainWindow *mainwindow, QWidget *parent);
    enum Stage {
        Welcomm,
        Policy,
        Wiki,
        Settings
    };
    ~Loading();
    void Draw(int timer, bool isFirst = false);
    void DrawStage(Stage s);
protected:
    void resizeEvent(QResizeEvent *event) override;


private:
    Ui::Loading *ui;
    MainWindow *m_mainwindow;
    bool isBackgroundVisible;
    QLabel *bgLabel = nullptr;
};

#endif // LOADING_H
