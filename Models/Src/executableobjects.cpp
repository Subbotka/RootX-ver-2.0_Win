#include "Models/Head/executableobjects.h"
#include "ui_executableobjects.h"
#include "Models/Head/mainwindow.h"
#include "ui_mainwindow.h"
#include "Models/Head/imagecontroller.h"
#include "Models/Lib/QAdvanceddocking/DockManager.h"

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QPixmap>

#include "TGraph.h"
#include "TPaveText.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TPaveText.h"
#include "TCanvas.h"

#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TH1.h>
#include "TF1.h"
#include <TVirtualPad.h>
#include <TGraph2D.h>
#include <vector>
#include <algorithm>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>
#include <QGroupBox>
#include "TF1.h"
#include "Models/Head/filemanager.h"
#include "Models/Head/editor.h"
#include <QUrl>
#include <QUrlQuery>

ExecutableObjects::ExecutableObjects(FileManager::RootObj obj, MainWindow *mainwindow, QWidget *parent)
    : DraggableWidget(parent)
    , ui(new Ui::ExecutableObjects)
    , m_mainwindow(mainwindow)
    , canvasPNG(nullptr)
    , canvasWEB(nullptr)
    , robj(nullptr)
{
    ui->setupUi(this);

    ui->title_label->setText(obj.fileName);
    m_namefile = obj.fileName;
    m_filePath = obj.filePath;
    isOpenEditor = false;

    tempfile.setFileTemplate(QDir::tempPath() + "/rc_tmp_XXXXXX.png");
    ExecutableObjects::last = nullptr;
}

ExecutableObjects::~ExecutableObjects()
{
    delete ui;
}

void ExecutableObjects::setupEditPanel()
{
    if (isOpenEditor) return;
    isOpenEditor = true;

    QTimer::singleShot(0, this, [this]() {
        m_mainwindow->ui->imagelabel->hide();
        m_mainwindow->ui->tabcountry->hide();

        if (m_mainwindow->m_dockManager) m_mainwindow->setEditorVisible(true);

        if (m_mainwindow->m_objDock)
            m_mainwindow->m_objDock->takeWidget();
        if (m_mainwindow->m_consoleDock)
            m_mainwindow->m_consoleDock->takeWidget();

        Editor *editorobj = new Editor(m_mainwindow->ui->Widget, this);
        m_mainwindow->ui->horizontalLayout->addWidget(editorobj);
        editorobj->attachMainPanels(m_mainwindow);
        editorobj->show();
    });
}





void ExecutableObjects::setupCanvas(TObject *obj, bool isSelect) {
    while (QLayoutItem *item = ui->horizontalLayout_2->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete item;
    }
    canvasPNG = nullptr;
    canvasWEB = nullptr;

    auto showSnapshot = [this](const QPixmap &src) {
        canvasPNG = new QLabel(this);
        canvasPNG->setPixmap(src);
        canvasPNG->setScaledContents(true);
        ui->horizontalLayout_2->addWidget(canvasPNG);
    };


    if (isSelect) {
        QString objName = obj ? QString::fromUtf8(obj->GetName()) : m_canvasName;
        if (objName.isEmpty())
            return;

        QDir dirEngine(QCoreApplication::applicationDirPath());
        dirEngine.cd("root_install/js");

        QString filePath = dirEngine.filePath("index.htm");
        QUrl engineUrl(filePath);
        QString target = objName + ";1";

        QUrlQuery query;
        query.addQueryItem("file", m_filePath);
        query.addQueryItem("item", target);
        query.addQueryItem("nobrowser", "true");
        engineUrl.setQuery(query);

        canvasWEB = new QWebEngineView(this);
        canvasWEB->setUrl(engineUrl);
        ui->horizontalLayout_2->addWidget(canvasWEB);
        return;
    }

    QString objName = m_canvasName;
    if (objName.isEmpty()) {
        qWarning() << "[PNG] нет имени объекта";
        return;
    }

    TFile *f = TFile::Open(m_filePath.toLocal8Bit().constData(), "READ");
    TObject *o = nullptr;
    if (f && !f->IsZombie())
        o = f->FindObjectAny(objName.toUtf8().constData());

    if (!o) {
        if (f) { f->Close(); delete f; }
        qWarning() << "[PNG] не найден объект" << objName << "в" << m_filePath;
        return;
    }

    if (!tempfile.isOpen()) {
        tempfile.setAutoRemove(true);
        tempfile.open();
    }

    QString path = tempfile.fileName();


    TCanvas *canvas = dynamic_cast<TCanvas*>(o);
    if (canvas) {
        canvas->SaveAs(path.toStdString().c_str());
    } else {
        TCanvas tmp("rc_tmp_canvas", "rc_tmp_canvas", 800, 600);
        o->Draw();
        tmp.SaveAs(path.toStdString().c_str());
    }

    if (f) { f->Close(); delete f; }

    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "[PNG] не удалось загрузить" << path;
        return;
    }

    showSnapshot(pixmap);
}

void ExecutableObjects::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->pos().x() > width() - 20 || event->pos().y() > height() - 20) {
            m_resizing = true;
            m_resizeStartPos = event->globalPosition().toPoint();
            m_originalGeometry = geometry();
        } else {
            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - pos();
        }
        event->accept();
        FileManager::RootObj obj = {m_namefile,m_filePath};
        m_mainwindow->catchTarget(obj, this);
        if (ExecutableObjects::last == this) return;
        if (ExecutableObjects::last)
            ExecutableObjects::last->setupCanvas(ExecutableObjects::last->robj, false);
        ExecutableObjects::last = this;
        setupCanvas();
    }
}
void ExecutableObjects::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setupEditPanel();
    }
}

void ExecutableObjects::on_close_button_clicked()
{
    this->close();
    this->deleteLater();
}

