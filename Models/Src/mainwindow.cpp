#include "Models/Head/mainwindow.h"
#include "./ui_mainwindow.h"
#include "ui_executableobjects.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QDockWidget>
#include <QCursor>
#include <QMouseEvent>
#include <QString>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QCloseEvent>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include "Models/Head/imagecontroller.h"
#include "Models/Head/elements.h"
#include "Models/Head/notify.h"
#include "Models/Head/basemenu.h"
#include "Models/Head/loading.h"
#include "Models/Head/executableobjects.h"
#include "Models/Lib/Spreadsheet/spreadsheet.h"
#include "Models/Head/sideobjects.h"
#include "Models/Lib/QAdvanceddocking/DockManager.h"
#include "Models/Lib/QAdvanceddocking/DockAreaWidget.h"
#include "Models/Head/tooltabbar.h"
#ifdef Q_OS_WIN
#include "Models/Lib/KodeTerm/KodoTerm.hpp"
#include<QThread>
#endif



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lastObjDataCount(0)
    , activeBtnIndex(-1)
    , btnPressed(false)
    , m_object(nullptr)
    , m_element(nullptr)
    , m_tab(nullptr)
{
    ui->setupUi(this);

    originalPos = ui->imagelabel->pos();
    // Loading *loadingScreen = new Loading(this, this);
    // loadingScreen->ShowTimerWidget(4);
    m_imageController = new ImageController(this);
    m_imageController->setupHandAction(ui->actionhanddd, ui->imagelabel);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout->setSpacing(2);


    // --- Для переноса объектов на поле
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateFileList);
    updateTimer->start(100);
    this->installEventFilter(this);

    m_tab = new ToolTabBar(this);
    ui->verticalLayout_6->addWidget(m_tab);
    // ui->verticalLayout_7->setStretch(1,50);
    // ui->verticalLayout_7->setStretch(0,1);
    m_tab->show();

#ifdef Q_OS_WIN
    console = new KodoTerm(this);
    QFont font("Consolas", 11);
    font.setStyleHint(QFont::Monospace);
    console->setProgram("cmd.exe");
    console->setTheme(TerminalTheme::loadKonsoleTheme(":/KodoTermThemes/konsole/DarkPastels.colorscheme"));
    console->setArguments(QStringList() << "/K" << "chcp" << "65001");
    ui->horizontalLayout_6->addWidget(console);
    console->setMinimumSize(600, 200);
#endif


}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_object;
}



void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}


void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!m_docksCreated) {
        m_docksCreated = true;
        QTimer::singleShot(0, this, [this]() { createDocks(); });
    }
}



void MainWindow::closeEvent(QCloseEvent* event)
{

    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    if (m_dockManager) {
        settings.setValue("dockManagerState", m_dockManager->saveState());
    }

    settings.endGroup();
    settings.sync();

    event->accept(); // Все программа 200
}



void MainWindow::createDocks()
{
    if (!m_dockManager) {
        m_dockManager = new ads::CDockManager();
        m_dockManager->setStyleSheet(
            "ads--CAutoHideTab { background-color: #ffeb99; color: #000000; }"
            "ads--CAutoHideTab:hover, ads--CAutoHideTab:selected { background-color: #ffeb99; }"
            "ads--CAutoHideSideBar { background-color: #2b2b2b; }"
            "ads--CDockAreaTitleBar { background-color: #ffeb99; color: #000000; }"
            "ads--CDockWidgetTab { background-color: #ffeb99; color: #000000; }"
            "ads--CDockWidgetTab[activeTab=\"true\"] { background-color: #ffeb99; color: #000000; }");
    }
    ui->horizontalLayout->addWidget(m_dockManager);
    auto* canvasDock = new ads::CDockWidget("Поле");
    canvasDock->setObjectName("canvasDock");
    canvasDock->setWidget(ui->imagelabel);
    canvasDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);

    m_objDock = new ads::CDockWidget("Список объектов");
    m_objDock->setObjectName("objDock");
    m_objDock->setWidget(ui->rightPanel_widget);
    m_objDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);

    m_consoleDock = new ads::CDockWidget("Терминал");
    m_consoleDock->setObjectName("consoleDock");
    m_consoleDock->setWidget(ui->consoleWidget);
    m_consoleDock->setFeature(ads::CDockWidget::DockWidgetClosable, true);

    ads::CDockAreaWidget* canvasArea = m_dockManager->setCentralWidget(canvasDock);
    if (canvasArea) {
        m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_objDock, canvasArea);
        m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_consoleDock, canvasArea);
    }

    ui->horizontalLayout->addWidget(m_dockManager);

    QSettings settings;
    settings.beginGroup("MainWindow");
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    if (settings.contains("dockManagerState") && m_dockManager) {
        m_dockManager->restoreState(settings.value("dockManagerState").toByteArray());
    }

    settings.endGroup();


#ifdef Q_OS_WIN
    if (console)
        console->start();
#endif
}

void MainWindow::setEditorVisible(bool open)
{
    if (open) {
        if (m_dockManager)
            m_dockManager->hideManagerAndFloatingWidgets();
    } else {
        if (m_dockManager) {
            m_dockManager->show();
            m_dockManager->raise();
        }
    }
}

void MainWindow::restoreMainPanels()
{
    if (m_objDock)
        m_objDock->setWidget(ui->rightPanel_widget);
    if (m_consoleDock)
        m_consoleDock->setWidget(ui->consoleWidget);
    setEditorVisible(false);
}

FileManager::RootObj tempData;

// Логика отсвечивания
void MainWindow::DrawClue(bool show) {
    if (QVariantAnimation *oldAnim = ui->imagelabel->findChild<QVariantAnimation*>("borderAnim")) {
        delete oldAnim;
    }

    int Alpha = ui->imagelabel->property("AlphaValue").isValid() ? ui->imagelabel->property("AlphaValue").toInt() : 0;
    int targetAlpha = show ? 255 : 0;

    if (Alpha == targetAlpha) {
        if (!show) {
            if (QLabel *lbl = ui->imagelabel->findChild<QLabel*>("hintLabel")) delete lbl;
        }
        return;
    }

    QVariantAnimation *anim = new QVariantAnimation(ui->imagelabel);
    anim->setObjectName("borderAnim");
    anim->setDuration(300);
    anim->setStartValue(Alpha);
    anim->setEndValue(targetAlpha);
    anim->setEasingCurve(QEasingCurve::OutQuad);

    QObject::connect(anim, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        int alpha = value.toInt();
        ui->imagelabel->setProperty("AlphaValue", alpha);

        QLabel *lbl = ui->imagelabel->findChild<QLabel*>("hintLabel");

        if (alpha == 0) {
            if (lbl) delete lbl;
        } else {
            if (!lbl) {
                lbl = new QLabel(ui->imagelabel);
                lbl->setObjectName("hintLabel");
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setText("Необходимо перенести для создания объекта");
                lbl->setAttribute(Qt::WA_TransparentForMouseEvents);
            }

            lbl->setGeometry(ui->imagelabel->rect());

            QString style = QString(
                                "QLabel {"
                                "  border: 3px dashed rgba(128, 128, 128, %1);"
                                "  background-color: rgba(128, 128, 128, %2);"
                                "  color: rgba(255, 255, 255, %1);"
                                "  font-weight: bold;"
                                "  font-size: 14pt;"
                                "}"
                                ).arg(alpha).arg((30 * alpha) / 255);

            lbl->setStyleSheet(style);
            lbl->show();
        }
    });

    QObject::connect(anim, &QVariantAnimation::finished, this, [this, show]() {
        if (!show) {
            if (QLabel *lbl = ui->imagelabel->findChild<QLabel*>("hintLabel")) delete lbl;
            ui->imagelabel->setProperty("AlphaValue", 0);
        }
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
        QPoint globalPos = QCursor::pos();
        QPoint localPos = ui->imagelabel->mapFromGlobal(globalPos);
        DrawClue(false);
        if (ui->imagelabel->rect().contains(localPos) && btnPressed && activeBtnIndex != -1) {
            if (statusActivWidget) {
                tracer_file(ui->imagelabel, tempData);
                tempData = FileManager::RootObj{};
                statusActivWidget = false;
            }
            btnPressed = false;
            activeBtnIndex = -1;
            return true;
        }
        btnPressed = false;
        activeBtnIndex = -1;
    }

    return QMainWindow::eventFilter(obj, event);
}



void MainWindow::updateFileList() {
    QString buttonStyle =
        "QPushButton {"
        "  background-color: #868789;"
        "  border: 1px solid #757678;"
        "  border-radius: 4px;"
        "  text-align: left;"
        "  padding: 6px 12px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: #FFFFFF;"
        "}"
        "QPushButton:hover {"
        "  background-color: #757678;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #636466;"
        "}";
    if (m_filemanager.ObjDataCount > lastObjDataCount) {
        for (int i = lastObjDataCount; i < m_filemanager.ObjDataCount; ++i) {
            QString name = "    " + m_filemanager.ObjectList[i].fileName + " | # " + QString::number(i + 1);
            QString filePath = m_filemanager.ObjectList[i].filePath;
            QPushButton* btn = new QPushButton(name);
            btn->setFixedHeight(60);
            btn->setStyleSheet(buttonStyle);
            btn->setContextMenuPolicy(Qt::CustomContextMenu);
            if (m_filemanager.ObjectList[i].type == Executable) {
                btn->setIcon(QIcon(":/source/img/ocircle.png"));
                btn->setToolTip("Исполняемый файл");
            }
            else if (m_filemanager.ObjectList[i].type == Side) {
                btn->setIcon(QIcon(":/source/img/bcircle.png"));
                btn->setToolTip("Внешний файл");
            }
            QMenu* menu = new QMenu(btn);
            QAction* delAction = menu->addAction("Закрыть");

            connect(delAction, &QAction::triggered, this, [this, btn]() {
                int actualIndex = ui->verticalLayout_4->indexOf(btn);
                if (actualIndex != -1) {
                    ui->verticalLayout_4->removeWidget(btn);
                    btn->deleteLater();
                    m_filemanager.DelObject(actualIndex);
                    for (int idx = 0; idx < ui->verticalLayout_4->count(); ++idx) {
                        QPushButton* remainingBtn = qobject_cast<QPushButton*>(ui->verticalLayout_4->itemAt(idx)->widget());
                        if (remainingBtn) {
                            QString text = remainingBtn->text();
                            if (text.contains(" | # ")) {
                                QString baseName = text.split(" | # ").first();
                                remainingBtn->setText(baseName + " | # " + QString::number(idx + 1));
                            }
                        }
                    }
                    lastObjDataCount = m_filemanager.ObjectList.size();
                }
            });

            connect(btn, &QPushButton::customContextMenuRequested, [menu](const QPoint&) {
                menu->exec(QCursor::pos());
            });
            Type type = m_filemanager.ObjectList[i].type;
            connect(btn, &QPushButton::pressed, this, [this, btn, name, filePath, type]() {
                statusActivWidget = true;
                if (tempData.fileName.isEmpty()) {
                    qDebug() << "Запись пустая";
                }
                tempData = {name, filePath, type};
                btnPressed = true;
                DrawClue(true);
                activeBtnIndex = ui->verticalLayout_4->indexOf(btn);
            });

            int insertIndex = 0;
            for (int idx = 0; idx < ui->verticalLayout_4->count(); ++idx) {
                QWidget* w = ui->verticalLayout_4->itemAt(idx)->widget();
                if (w && w->objectName() == "ProjectPanel") {
                    break;
                }
                insertIndex++;
            }
            ui->verticalLayout_4->insertWidget(insertIndex, btn);
        }
        lastObjDataCount = m_filemanager.ObjDataCount;
    }

    if (m_filemanager.ProjDataCount > lastProjDataCount) {
        for (int p = lastProjDataCount; p < m_filemanager.ProjDataCount; ++p) {
            const auto& proj = m_filemanager.ProjectList[p];

            QWidget* projPanel = new QWidget();
            projPanel->setObjectName("ProjectPanel");
            projPanel->setStyleSheet(
                "QWidget#ProjectPanel {"
                "  background-color: #B5B0A5;" /* Плотный серый с мягким теплым подтоном */
                "  border: 1px solid #9E998F;"
                "  border-radius: 6px;"
                "}"
                );

            QVBoxLayout* projLayout = new QVBoxLayout(projPanel);
            projLayout->setContentsMargins(15, 15, 15, 15);
            projLayout->setSpacing(5);

            QLabel* titleLabel = new QLabel(proj.projName);
            titleLabel->setStyleSheet("background-color: transparent; border: none; font-size: 15px; font-weight: bold; color: #2C2C2C;");
            projLayout->addWidget(titleLabel);


            QWidget* buttonsContainer = new QWidget();
            buttonsContainer->setStyleSheet("border: none; background-color: transparent;");

            QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsContainer);
            buttonsLayout->setContentsMargins(15, 5, 0, 5);
            buttonsLayout->setSpacing(10);

            for (size_t o = 0; o < proj.objList.size(); ++o) {
                const auto& subObj = proj.objList[o];
                QString subName = subObj.fileName + " [P]";
                QString subPath = subObj.filePath;

                QPushButton* subBtn = new QPushButton(subName);
                subBtn->setFixedHeight(40);
                subBtn->setStyleSheet(buttonStyle); // Тот же стиль кнопок
                if (subObj.type == Executable) {
                    subBtn->setIcon(QIcon(":/source/img/ocircle.png"));
                    subBtn->setToolTip("Исполняемый файл");
                }
                else if (subObj.type == Side) {
                    subBtn->setIcon(QIcon(":/source/img/bcircle.png"));
                    subBtn->setToolTip("Внешний файл");
                }
                Type type = subObj.type;
                connect(subBtn, &QPushButton::pressed, this, [this, o, subName, subPath, type]() {
                    statusActivWidget = true;
                    if (tempData.fileName.isEmpty()) {
                        qDebug() << "Запись пустая";
                    }
                    tempData = {subName, subPath, type};
                    btnPressed = true;
                    activeBtnIndex = static_cast<int>(o);
                });

                buttonsLayout->addWidget(subBtn);
            }
            projLayout->addWidget(buttonsContainer);
            ui->verticalLayout_4->addWidget(projPanel);
        }
        lastProjDataCount = m_filemanager.ProjDataCount;
    }
}



void MainWindow::on_pushButton_clicked() {
    m_filemanager.ShowFileFolder();
}

void MainWindow::on_action_2_triggered() {

}

void MainWindow::on_action_11_triggered() {

}

void MainWindow::tracer_file(QWidget* parent,  FileManager::RootObj obj) {
    if (!parent) return;
    qDebug() << obj.type;
    if (obj.type == Executable) {
        ExecutableObjects *exe = new ExecutableObjects(obj, this);
        exe->setParent(parent);
        exe->setGeometry(10, 10, 200, 100);
        exe->show();
        exe->setupCanvas();
    }
    else if (obj.type == Side) {
        SideObjects *side = new SideObjects(parent, this, obj);
        side->setGeometry(10, 10, 200, 100);
        side->show();
    }
}

void MainWindow::catchTarget( const FileManager::RootObj &obj, QWidget *parent) {
    m_obj = obj;
    m_target = parent;
    ui->selected_action->setText("Выбрано: " + m_obj.fileName);
}





void MainWindow::on_copy_action_triggered()
{
    if (m_obj.fileName.isEmpty()) return;
    QString temp =  m_obj.fileName;
    m_obj.fileName=+ " Копия";
    if (m_obj.fileName != temp) {
        tracer_file(ui->imagelabel, m_obj);
        m_obj.fileName = temp;
    }
}


void MainWindow::on_forward_action_triggered()
{
    if (m_target) {
        m_target->raise();
    }
    else {
        Notify *notify = new Notify();
        notify->Show("Ошибка", "Объект не найден !", 3, ui->imagelabel);
    }
}

void MainWindow::on_back_action_triggered()
{
    if (m_target) {
        m_target->lower();
    }
    else {
        Notify *notify = new Notify();
        notify->Show("Ошибка", "Объект не найден !", 3, ui->imagelabel);
    }
}


void MainWindow::on_clear_action_triggered()
{
    QList<QWidget*> children = ui->imagelabel->findChildren<QWidget*>();
    for (QWidget* child : children) {
        child->close();
        child->deleteLater();
    }
    m_obj = FileManager::RootObj();
    m_target = nullptr;
    Notify *notify = new Notify();
    notify->Show("Отчистка", "Все объекты и элементы удалены !", 3 ,ui->imagelabel);

}


void MainWindow::on_reducertab_button_clicked()
{
    if (ui->tabcountry->isVisible()) {
        ui->tabcountry->hide();
        ui->reducertab_button->setIcon(QIcon(":/source/img/bullet_arrow_up.png"));
        return;
    }
    ui->tabcountry->show();
    ui->reducertab_button->setIcon(QIcon(":/source/img/bullet_arrow_down .png"));
}

