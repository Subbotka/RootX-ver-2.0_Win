#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include "TFile.h"
#include <QTreeWidgetItem>
#include "Models/Lib/Scintilla/ScintillaEdit.h"
#include <QLabel>
#include <QTextEdit>
#include "Models/Lib/QAdvanceddocking/DockAreaWidget.h"
#include "Models/Lib/QPropertybrowser/qtvariantproperty.h"
class ExecutableObjects;
class MainWindow;
class TObject;
class TPad;
class TCanvas;
namespace Ui {
class Editor;
}
namespace ads {
class CDockManager;
}

class Editor : public QWidget
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr, ExecutableObjects *obj = nullptr);
    ~Editor();
    void attachMainPanels(MainWindow *mw);
    void detachMainPanels();

private slots:
    void on_close_button_clicked();
    void on_pushButton_2_clicked();
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void on_add_button_clicked();
    void on_update_button_clicked();
    void on_save_button_clicked();
    void on_lockcode_button_clicked();
    void onCharAdded(int ch);
    void updateBrackets();
    void updateLineNumberWidth();
    void restyle(long upTo);
    void onPropertyValueChanged(QtProperty *property, const QVariant &value);

private:
    Ui::Editor *ui;
    ExecutableObjects *exeobj;
    struct Item {
        QString name;
        QString iconPath;
        std::function<void(TFile*, const QString&)> createAction;
    };
    struct Category {
        QString name;
        QVector<Item> items;
    };
    QVector<Item> itemsGistogram;
    QVector<Item> itemsGraph;
    QVector<Item> itemsShapes;
    QVector<Category> category;
    void initData();
    void AddCategotyList();
    void setupTree();
    void setupCanvas(TObject *robj = nullptr);
    void setupCodeEditor(TObject *robj = nullptr);
    void setupEditor(TObject *robj = nullptr);
    bool canEditObject(TObject *obj);
    void addDirectoryItems(TDirectory *dir, QTreeWidgetItem *parentItem);
    bool canDrawinCanvas(TObject *obj);
    TObject *findObjectOwnerPad(TObject *obj);
    TObject *getCanvas(TObject *obj);
    QtVariantPropertyManager *m_variantManager = nullptr;
    QtVariantEditorFactory   *m_variantFactory  = nullptr;
    ScintillaEdit *codeEditor;
    TObject *drawobj;
    TFile *m_openFile;
    QTreeWidgetItem *m_currentTreeItem = nullptr;
    ads::CDockManager *m_dockManager;
    QWidget *editorPropety;
    ads::CDockWidget *m_editorDock = nullptr;
    bool m_panelsAttached = false;
    bool inEditAction = false;
    QtProperty *mainProp;
    QtVariantProperty *m_nameObjProp;

};



#endif // EDITOR_H
