#ifndef EXECUTABLEExecutableObjects_H
#define EXECUTABLEExecutableObjects_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include "Models/Head/filemanager.h"
#include "Models/Lib/draggablewidget.h"
#include "QTemporaryFile"
#include "QWebEngineView"
class MainWindow;

namespace Ui {
class ExecutableObjects;
}

class ExecutableObjects : public DraggableWidget
{
    Q_OBJECT

public:
    Ui::ExecutableObjects *ui;
    explicit ExecutableObjects(FileManager::RootObj obj, MainWindow *mainwindow = nullptr, QWidget *parent = nullptr);
    ~ExecutableObjects();
    void setupCanvas(TObject *obj = nullptr, bool isSelect = true);
    QString m_namefile;
    QString m_filePath;
    QString m_canvasName;
    bool isOpenEditor;
    MainWindow *m_mainwindow;
    TObject *robj;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    inline static ExecutableObjects *last;

private slots:

    void on_close_button_clicked();

private:
    QTemporaryFile tempfile;
    QLabel *canvasPNG;
    QWebEngineView *canvasWEB;
    void setupEditPanel();
};

#endif
