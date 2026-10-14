#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QResizeEvent>//
#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>
#include <QLabel>
#include <QTextEdit>
#include <QPoint>
#include <QFileDialog>
#include "filemanager.h"
#include <TFile.h>
#include <QVector>
#include "Models/Lib/KodeTerm/KodoTerm.hpp"


class ExecutableObjects;
class Elements;
class FileManager;
class ToolTabBar;
class ImageController;
class BaseMenu;
namespace ads {
class CDockManager;
class CDockWidget;
}
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~MainWindow();
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow *ui;
    ImageController *m_imageController;
    void tracer_file(QWidget* parent, FileManager::RootObj obj);
    void catchTarget(const FileManager::RootObj &obj, QWidget *parent);
    auto GetFileManager() { return m_filemanager;}
    bool showLeftPanel = false;
    FileManager m_filemanager;
    ads::CDockManager *m_dockManager = nullptr;
    ads::CDockWidget *m_objDock = nullptr;
    ads::CDockWidget *m_consoleDock = nullptr;
    ToolTabBar *m_tab;
    void setEditorVisible(bool open);
    void restoreMainPanels();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;//
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_pushButton_clicked();
    void updateFileList();
    void on_action_2_triggered();
    void on_action_11_triggered();

    void on_copy_action_triggered();
    void on_forward_action_triggered();
    void on_back_action_triggered();
    void on_clear_action_triggered();


    void on_reducertab_button_clicked();

private:
        KodoTerm *console = nullptr;
    void AddCategotyList();
    void createDocks();
    bool m_docksCreated = false;
    void DrawClue(bool show);
    QPoint originalPos;
    QVBoxLayout* scrollLayout = nullptr;
    QTimer* updateTimer = nullptr;
    int lastObjDataCount = 0;
    int lastProjDataCount = 0;
    bool btnPressed = false;
    bool showRightPanel = true;
    bool statusActivWidget = false;
    int activeBtnIndex = -1;
    FileManager::RootObj m_obj;
    QWidget *m_target;
    Elements *m_element;

    ExecutableObjects *m_object;

    static int base_size;

};
#endif // MAINWINDOW_H
