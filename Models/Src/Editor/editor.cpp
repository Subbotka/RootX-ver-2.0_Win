#include "Models/Head/editor.h"
#include "Models/Head/executableobjects.h"
#include "Models/Head/mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_editor.h"
#include <QPushButton>
#include <QToolButton>
#include "Models/Head/notify.h"
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TH1.h>
#include <TAxis.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TGraphBentErrors.h>
#include <TGraphPolar.h>
#include <TGraph2D.h>
#include <TGraphQQ.h>
#include <TBox.h>
#include <TEllipse.h>
#include <TArc.h>
#include <TCurlyLine.h>
#include <TCrown.h>
#include <TCanvas.h>
#include <TAttLine.h>
#include <TAttFill.h>
#include <TAttMarker.h>
#include <TAttText.h>
#include <TFrame.h>
#include <TColor.h>
#include <TF1.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TDirectory.h>
#include <TClass.h>
#include <TMethod.h>
#include <TMethodArg.h>
#include <TMethodCall.h>
#include <TNamed.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TPaveLabel.h>
#include <TText.h>
#include <TLegend.h>
#include <QFont>
#include <QColor>
#include <QTimer>
#include <QLayout>
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <sstream>
#include <QSet>
#include <memory>
#include "Models/Lib/flowlayout.h"
#include "Models/Lib/QAdvanceddocking/DockManager.h"
#include "Models/Lib/QAdvanceddocking/DockAreaWidget.h"
#include "Models/Lib/QPropertybrowser/qtpropertymanager.h"
#include "Models/Lib/QPropertybrowser/qtvariantproperty.h"
#include "Models/Lib/QPropertybrowser/qttreepropertybrowser.h"
#include <QUrl>
#include <QUrlQuery>
#include <QRegularExpression>
#include <QSet>
#include "TMultiGraph.h"
#include <QtConcurrent>
#include <QFutureWatcher>

Editor::Editor(QWidget *parent, ExecutableObjects *obj)
    : QWidget(parent)
    , ui(new Ui::Editor)
    , exeobj(obj)
    , codeEditor(nullptr)
    , drawobj(nullptr)
    , m_openFile(nullptr)
    , editorPropety(nullptr)
    , mainProp(nullptr)
    , m_nameObjProp(nullptr)

{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    AddCategotyList();
    // === Поток 1: Настройка дерева ===
    auto* treeWatcher = new QFutureWatcher<void>(this);
    connect(treeWatcher, &QFutureWatcher<void>::finished, this, [treeWatcher]() {
        treeWatcher->deleteLater();
    });


    QFuture<void> treeFuture = QtConcurrent::run(&Editor::setupTree, this);
    treeWatcher->setFuture(treeFuture);


    // // === Поток 2: Добавление категорий ===
    // auto* itemDataWatcher = new QFutureWatcher<void>(this);
    // connect(itemDataWatcher, &QFutureWatcher<void>::finished, this, [itemDataWatcher]() {
    //     itemDataWatcher->deleteLater();
    // });

    // QFuture<void> itemDataFuture = QtConcurrent::run(&Editor::AddCategotyList, this);
    // itemDataWatcher->setFuture(itemDataFuture);


    ui->title_lable->setText(exeobj->m_namefile);

    ads::CDockManager::setAutoHideConfigFlags({
        ads::CDockManager::AutoHideFeatureEnabled,
        ads::CDockManager::DockAreaHasAutoHideButton,
        ads::CDockManager::AutoHideHasMinimizeButton,
        ads::CDockManager::AutoHideCloseOnOutsideMouseClick});

    m_dockManager = new ads::CDockManager(this);
    m_dockManager->setStyleSheet(
        "ads--CAutoHideTab { background-color: #ffeb99; color: #000000; }"
        "ads--CAutoHideTab:hover, ads--CAutoHideTab:selected { background-color: #ffeb99; }"
        "ads--CAutoHideSideBar { background-color: #2b2b2b; }"
        "ads--CDockAreaTitleBar { background-color: #ffeb99; color: #000000; }"
        "ads--CDockWidgetTab { background-color: #ffeb99; color: #000000; }"
        "ads--CDockWidgetTab[activeTab=\"true\"] { background-color: #ffeb99; color: #000000; }");

    auto* centerDock = new ads::CDockWidget("Рабочая область");
    centerDock->setWidget(ui->fill_widget);

    auto* navDock = new ads::CDockWidget("Навигация");
    navDock->setObjectName("navDock");
    navDock->setWidget(ui->left_widget);

    auto* dock = new ads::CDockWidget("Панель");
    dock->setObjectName("panelDock");
    dock->setWidget(ui->right_widget);

    centerDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    navDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    dock->setFeature(ads::CDockWidget::DockWidgetClosable, false);

    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, centerDock);
    m_dockManager->addDockWidget(ads::RightDockWidgetArea, dock);
    m_dockManager->addDockWidget(ads::LeftDockWidgetArea, navDock);

    QTimer::singleShot(0, this, [this]() {
        if (ads::CDockWidget *d = m_dockManager->findDockWidget("navDock"))
            d->setAutoHide(true, ads::SideBarLeft);
    });

    if (QLayoutItem *li = ui->verticalLayout_3->takeAt(0)) {
        QWidget *placeholder = li->widget();
        delete li;
        if (placeholder)
            placeholder->deleteLater();
    }
    ui->verticalLayout_3->addWidget(m_dockManager);

    QSettings settings;
    settings.beginGroup("Editor");
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    if (settings.contains("dockManagerState") && m_dockManager) {
        m_dockManager->restoreState(settings.value("dockManagerState").toByteArray());
    }

    settings.endGroup();
}

Editor::~Editor()
{
    if (m_variantManager) {
        disconnect(m_variantManager, nullptr, this, nullptr);
        m_variantManager->deleteLater();
        m_variantManager = nullptr;
    }
    if (m_openFile) {
        m_openFile->Close();
        delete m_openFile;
        m_openFile = nullptr;
    }

    delete ui;
}

void Editor::on_close_button_clicked()
{

    QSettings settings;
    settings.beginGroup("Editor");
    settings.setValue("geometry", saveGeometry());
    if (m_dockManager) {
        settings.setValue("dockManagerState", m_dockManager->saveState());
    }

    settings.endGroup();
    settings.sync();



    if (exeobj) {
        TObject *freshObj = gROOT->FindObject(exeobj->m_namefile.toUtf8().constData());
        if (!freshObj) {
            TIter next(gROOT->GetList());
            TObject *o;
            while ((o = next())) {
                if (QString::fromUtf8(o->GetName()).contains(exeobj->m_namefile)) {
                    freshObj = o;
                    break;
                }
            }
        }
        if (freshObj) {
            exeobj->setupCanvas(freshObj);
        } else if (drawobj) {
            exeobj->setupCanvas(drawobj);
        }
    }

    if (exeobj->isOpenEditor) {
        detachMainPanels();
        exeobj->m_mainwindow->restoreMainPanels();
        //exeobj->m_mainwindow->ui->toolBar->show();
        exeobj->m_mainwindow->ui->imagelabel->show();
        this->close();
        exeobj->isOpenEditor = false;
        return;
    }
    exeobj->isOpenEditor = false;
    this->close();
}

void Editor::attachMainPanels(MainWindow *mw)
{
    if (m_panelsAttached || !mw || !mw->m_dockManager)
        return;
    m_panelsAttached = true;

    auto *objDock = new ads::CDockWidget("Список объектов");
    objDock->setObjectName("editorObjectDock");
    objDock->setWidget(mw->ui->rightPanel_widget);
    objDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    m_dockManager->addDockWidget(ads::LeftDockWidgetArea, objDock);

    auto *consoleDock = new ads::CDockWidget("Терминал");
    consoleDock->setObjectName("editorConsoleDock");
    consoleDock->setWidget(mw->ui->consoleWidget);
    consoleDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    m_dockManager->addDockWidget(ads::BottomDockWidgetArea, consoleDock);

    QTimer::singleShot(0, this, [this]() {
        if (ads::CDockWidget *d = m_dockManager->findDockWidget("editorObjectDock"))
            d->setAutoHide(true, ads::SideBarLeft);
        if (ads::CDockWidget *d = m_dockManager->findDockWidget("editorConsoleDock"))
            d->setAutoHide(true, ads::SideBarBottom);
    });
}

void Editor::detachMainPanels()
{
    if (!m_panelsAttached)
        return;
    m_panelsAttached = false;

    if (ads::CDockWidget *d = m_dockManager->findDockWidget("editorObjectDock")) {
        d->takeWidget();
        m_dockManager->removeDockWidget(d);
        d->deleteLater();
    }
    if (ads::CDockWidget *d = m_dockManager->findDockWidget("editorConsoleDock")) {
        d->takeWidget();
        m_dockManager->removeDockWidget(d);
        d->deleteLater();
    }
}



void Editor::setupCanvas(TObject *robj) {
    qDebug() << "[CANVAS] setupCanvas begin";
    if (!robj) {
        ui->webEngineView->stop();
        ui->webEngineView->setUrl(QUrl("about:blank"));
        qDebug() << "[CANVAS] url cleared (about:blank)";
        return;
    }

    exeobj->m_canvasName = QString::fromUtf8(robj->GetName());
    exeobj->setupCanvas(robj);

    QDir dirEngine(QCoreApplication::applicationDirPath());
    dirEngine.cd("root_install/js");
    QString filePath = dirEngine.filePath("index.htm");

    QUrl engineUrl(filePath);
    QString target = (QString)robj->GetName() + ";1";
    QUrlQuery query;
    query.addQueryItem("file", exeobj->m_filePath);
    query.addQueryItem("item", target);
    query.addQueryItem("nobrowser", "true");
    engineUrl.setQuery(query);
    ui->webEngineView->setUrl(engineUrl);
    qDebug() << "[CANVAS] url set:" << engineUrl.toString();
}

bool Editor::canDrawinCanvas(TObject *obj) {
    if (!obj) return false;
    return obj->InheritsFrom("TH1")
           || obj->InheritsFrom("THnSparse")
           || obj->InheritsFrom("THStack")
           || obj->InheritsFrom("TGraph")
           || obj->InheritsFrom("TGraph2D")
           || obj->InheritsFrom("TMultiGraph")
           || obj->InheritsFrom("TF1")
           || obj->InheritsFrom("TTree")
           || obj->InheritsFrom("TPie")
           || obj->InheritsFrom("TLegend")
           || obj->InheritsFrom("TPaveText")
           || obj->InheritsFrom("TPaveStats")
           || obj->InheritsFrom("TASImage")
           || obj->InheritsFrom("TCanvas")
           || obj->InheritsFrom("TFrame");
}

// ============================================================
//  Поиск канваса/пада для выбранного объекта
// ============================================================


void Editor::on_pushButton_2_clicked()
{
    if (ui->left_widget->isVisible()) {
        ui->left_widget->hide();
        return;
    }
    ui->left_widget->show();
}

void Editor::on_add_button_clicked()
{
    if (ui->left_widget->isVisible()) {
        ui->left_widget->hide();
        return;
    }
    ui->left_widget->show();
}

void Editor::on_update_button_clicked()
{
    setupTree();
}



void Editor::on_save_button_clicked()
{
    qDebug() << "[Save] == Entering on_save_button_clicked ==";
    if (!codeEditor) { qWarning() << "[Save] editor == null"; return; }
    if (!exeobj) { qWarning() << "[Save] exeobj == null"; return; }
    if (!drawobj) { qWarning() << "[Save] drawobj == null"; return; }
    if (exeobj->m_filePath.isEmpty()) { qWarning() << "[Save] m_filePath is EMPTY"; return; }

    if (m_openFile) {
        m_openFile->Close();
        delete m_openFile;
        m_openFile = nullptr;
    }

    ui->webEngineView->stop();
    ui->webEngineView->setUrl(QUrl("about:blank"));

    auto notify = [this](const QString &title, const QString &text) {
        (new Notify())->Show(title, text, 6, this);
    };

    QString appDir = QCoreApplication::applicationDirPath();
    QString rootExe = appDir + "/root_install/bin/root.exe";

    if (!QFileInfo::exists(rootExe)) {
        notify("Ошибка", "root.exe не найден рядом с приложением:\n" + rootExe);
        setupTree();
        return;
    }

    QString editorText = QString::fromLatin1(codeEditor->getText(codeEditor->textLength()));
    QString fname = "rootc_code";
    QRegularExpression re("\\bvoid\\s+([A-Za-z_]\\w*)\\s*\\(");
    QRegularExpressionMatch rm = re.match(editorText);
    if (rm.hasMatch())
        fname = rm.captured(1);

    QString injected;
    {
        static const QStringList keywords = {
            "return", "if", "for", "while", "else", "switch", "case", "goto",
            "sizeof", "using", "int", "double", "float", "char", "bool",
            "long", "short", "unsigned", "signed", "void", "auto", "const"
        };

        QSet<QString> alreadyDeclared;
        QRegularExpression declRe(
            "\\b([A-Za-z_]\\w*)\\s*\\*+\\s*([A-Za-z_]\\w*)\\b|"
            "\\b([A-Za-z_]\\w*)\\s+([A-Za-z_]\\w*)\\s*[;=]");
        QRegularExpressionMatchIterator di = declRe.globalMatch(editorText);
        while (di.hasNext()) {
            QRegularExpressionMatch dm = di.next();
            if (!dm.captured(2).isEmpty()) {
                alreadyDeclared.insert(dm.captured(2));
            } else if (!dm.captured(3).isEmpty() && !dm.captured(4).isEmpty()
                       && !keywords.contains(dm.captured(3))) {
                alreadyDeclared.insert(dm.captured(4));
            }
        }

        QSet<QString> injectedVars;
        QRegularExpression assignNewRe("\\b([A-Za-z_]\\w*)\\s*=\\s*new\\s+([A-Za-z_]\\w*)\\s*\\(");
        QRegularExpressionMatchIterator ai = assignNewRe.globalMatch(editorText);
        while (ai.hasNext()) {
            QRegularExpressionMatch am = ai.next();
            QString var = am.captured(1);
            QString type = am.captured(2);
            if (alreadyDeclared.contains(var) || injectedVars.contains(var)) continue;
            injectedVars.insert(var);
            injected += "// auto-declared (cstyle):\n";
            injected += type + " *" + var + " = nullptr;\n";
        }
    }

    QString objName = QString(drawobj->GetName());
    if (objName.isEmpty()) objName = "Graph";
    QString autoSave;
    {
        QString targetFile = QDir::fromNativeSeparators(exeobj->m_filePath);
        autoSave += "\n    // --- auto-save: объекты, нарисованные на канвасах ---\n";
        autoSave += "    TFile *asF = TFile::Open(\"" + targetFile + "\", \"UPDATE\");\n";
        autoSave += "    if (asF && !asF->IsZombie()) {\n";
        autoSave += "        TObject *asFound = nullptr;\n";
        autoSave += "        TIter asCi(gROOT->GetListOfCanvases()); TCanvas *asCv;\n";
        autoSave += "        while ((asCv = (TCanvas*)asCi()) && !asFound) {\n";
        autoSave += "            TIter asPi(asCv->GetListOfPrimitives()); TObject *asP;\n";
        autoSave += "            while ((asP = (TObject*)asPi()) && !asFound) {\n";
        autoSave += "                if (!asP) continue;\n";
        autoSave += "                bool sv = asP->InheritsFrom(\"TGraph\") || asP->InheritsFrom(\"TGraph2D\") || asP->InheritsFrom(\"TMultiGraph\") || asP->InheritsFrom(\"TH1\") || asP->InheritsFrom(\"TH2\") || asP->InheritsFrom(\"TH3\") || asP->InheritsFrom(\"TProfile\") || asP->InheritsFrom(\"TF1\") || asP->InheritsFrom(\"TTree\");\n";
        autoSave += "                if (sv) asFound = asP;\n";
        autoSave += "            }\n";
        autoSave += "        }\n";
        autoSave += "        if (asFound) {\n";
        autoSave += "            asF->Delete(\"" + objName + ";*\");\n";
        autoSave += "            asF->cd();\n";
        autoSave += "            asFound->Write(\"" + objName + "\", TObject::kOverwrite);\n";
        autoSave += "        }\n";
        autoSave += "        asF->Purge(); asF->Write(); asF->Close(); delete asF;\n";
        autoSave += "    }\n";
    }

    QByteArray macroContent = injected.toUtf8();
    if (rm.hasMatch()) {
        macroContent = (injected + editorText).toUtf8();
    } else {
        macroContent = ("void " + fname + "() {\n" + injected + editorText + autoSave + "\n}\n").toUtf8();
    }

    QString macroPath = QDir::tempPath() + "/" + fname + ".C";
    QFile macroFile(macroPath);
    if (!macroFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        notify("Ошибка", "Не удалось создать временный макрос:\n" + macroPath);
        setupTree();
        return;
    }
    macroFile.write(macroContent);
    macroFile.close();
    notify("Сборка", QString("Запускаю root.exe на временном макросе:\n%1").arg(macroPath));

    QProcess proc;
    proc.setProgram(rootExe);
    proc.setArguments(QStringList() << "-b" << "-l" << "-q" << macroPath);
    proc.start();
    bool finished = proc.waitForFinished(120000);
    if (!finished) {
        proc.kill();
        proc.waitForFinished(3000);
    }
    QByteArray out = proc.readAllStandardOutput();
    QByteArray err = proc.readAllStandardError();
    int code = proc.exitCode();

    QString outText = QString::fromUtf8(out).trimmed();
    QString errText = QString::fromUtf8(err).trimmed();
    if (errText.length() > 900) errText = errText.left(897) + "...";

    if (!finished) {
        notify("Ошибка", "root.exe завис на 120 секунд — процесс завершён принудительно.");
    } else if (code != 0) {
        notify("Ошибка выполнения макроса",
               QString("root.exe завершился с кодом %1.\n\n%2\n\n%3")
                   .arg(code)
                   .arg(errText.isEmpty() ? "(stderr пуст)" : errText)
                   .arg(outText.isEmpty() ? "" : outText));
    } else {
        notify("Готово",
               QString("Макрос выполнен (код 0).\n\n%1")
                   .arg(outText.isEmpty() ? "Обновляю дерево объектов..." : outText));
    }

    QFile::remove(macroPath);

    setupTree();
    if (drawobj)
        setupCanvas(drawobj);
}


