#ifndef SIDEOBJECTS_H
#define SIDEOBJECTS_H

#include <QWidget>
#include <QVector>
#include "Models/Lib/draggablewidget.h"
#include "Models/Head/filemanager.h"

class MainWindow;
class SpreadSheet;
namespace Ui {
class SideObjects;
}
class SideObjects : public DraggableWidget
{
    Q_OBJECT

public:
    explicit SideObjects(QWidget *parent = nullptr, MainWindow *mainwindow = nullptr, const FileManager::RootObj &obj = {});
    ~SideObjects();
    MainWindow *m_mainwindow;
    FileManager::RootObj m_obj;
private:
    void CSVToArray(const QString &filepath);

    QVector<QVector<QString>> data;
    Ui::SideObjects *ui;
    SpreadSheet *m_table;


};

#endif // SIDEOBJECTS_H
