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


void Editor::initData() {
    itemsGistogram.append({
        "TH1C", ":/new/prefix1/app_thing/histogram.png",
        [](TFile* file, const QString& objName) {
            TH1C *h = new TH1C(objName.toUtf8().constData(), objName.toUtf8().constData(), 100, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH1S", ":/new/prefix1/app_thing/histogram.png",
        [](TFile* file, const QString& objName) {
            TH1S *h = new TH1S(objName.toUtf8().constData(), objName.toUtf8().constData(), 100, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH1I", ":/new/prefix1/app_thing/histogram.png",
        [](TFile* file, const QString& objName) {
            TH1I *h = new TH1I(objName.toUtf8().constData(), objName.toUtf8().constData(), 100, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH1F", ":/new/prefix1/app_thing/histogram.png",
        [](TFile* file, const QString& objName) {
            TH1F *h = new TH1F(objName.toUtf8().constData(), objName.toUtf8().constData(), 100, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH1D", ":/new/prefix1/app_thing/histogram.png",
        [](TFile* file, const QString& objName) {
            TH1D *h = new TH1D(objName.toUtf8().constData(), objName.toUtf8().constData(), 100, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH2F", ":/new/prefix1/app_thing/kvadrat.png",
        [](TFile* file, const QString& objName) {
            TH2F *h = new TH2F(objName.toUtf8().constData(), objName.toUtf8().constData(), 50, -10, 10, 50, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });
    itemsGistogram.append({
        "TH3F", ":/new/prefix1/app_thing/kube.png",
        [](TFile* file, const QString& objName) {
            TH3F *h = new TH3F(objName.toUtf8().constData(), objName.toUtf8().constData(), 30, -10, 10, 30, -10, 10, 30, -10, 10);
            h->FillRandom("gaus", 1000);
            h->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete h;
        }
    });

    itemsGraph.append({
        "TGraph", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraph *g = new TGraph(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) g->SetPoint(i, i, sin(i * 0.1) * 100);
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraphErrors", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraphErrors *g = new TGraphErrors(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i, sin(i * 0.1) * 100);
                g->SetPointError(i, 0.1, 1.0);
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraphAsymmErrors", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraphAsymmErrors *g = new TGraphAsymmErrors(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i, sin(i * 0.1) * 100);
                g->SetPointError(i, 0.1, 0.1, 1.0, 1.0);
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraphBentErrors", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraphBentErrors *g = new TGraphBentErrors(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i, sin(i * 0.1) * 100);
                g->SetPointError(i, 0.1, 0.1, 1.0, 1.0, 0.01, 0.01);
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraphPolar", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraphPolar *g = new TGraphPolar(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i * 0.1, sin(i * 0.1) * 100);
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraph2D", ":/new/prefix1/app_thing/kube.png",
        [](TFile* file, const QString& objName) {
            TGraph2D *g = new TGraph2D(100);
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i, sin(i * 0.1) * 100, cos(i * 0.1) * 50);
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });
    itemsGraph.append({
        "TGraphQQ", ":/new/prefix1/app_thing/graph.png",
        [](TFile* file, const QString& objName) {
            TGraphQQ *g = new TGraphQQ();
            g->SetName(objName.toUtf8().constData());
            for (int i = 0; i < 100; i++) {
                g->SetPoint(i, i * 1.0, i * 1.0 + (rand() % 10 - 5));
            }
            g->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete g;
        }
    });

    itemsShapes.append({
        "TBox", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString& objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TBox *box = new TBox(0.2, 0.2, 0.8, 0.8);
            box->SetFillColor(kBlue);
            box->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });
    itemsShapes.append({
        "TEllipse", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString& objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TEllipse *ellipse = new TEllipse(0.5, 0.5, 0.3, 0.2);
            ellipse->SetFillColor(kRed);
            ellipse->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });
    itemsShapes.append({
        "TArc", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString& objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TArc *arc = new TArc(0.5, 0.5, 0.3);
            arc->SetFillColor(kGreen);
            arc->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });
    itemsShapes.append({
        "TCurlyLine", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString& objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TCurlyLine *cline = new TCurlyLine(0.1, 0.5, 0.9, 0.5, 0.1, 0.1);
            cline->SetWavy();
            cline->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });
    itemsShapes.append({
        "TCrown", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString& objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TCrown *crown = new TCrown(0.5, 0.5, 0.3, 0.4);
            crown->SetFillColor(kYellow);
            crown->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });
    itemsShapes.append({
        "TDiamond", ":/new/prefix1/app_thing/shapes.png",
        [](TFile* file, const QString &objName) {
            TCanvas *c = new TCanvas(objName.toUtf8().constData(), objName.toUtf8().constData(), 800, 600);
            TBox *diamond = new TBox(0.3, 0.3, 0.7, 0.7);
            diamond->SetFillColor(kMagenta);
            diamond->Draw();
            c->Write(objName.toUtf8().constData(), TObject::kOverwrite);
            delete c;
        }
    });

    category = {
        {"Гистограммы", itemsGistogram},
        {"Графы", itemsGraph},
        {"Фигуры", itemsShapes}
    };
}

void Editor::AddCategotyList() {
    initData();
    while (QLayoutItem* item = ui->verticalLayout_8->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < category.size(); ++i) {
        QPushButton *btnCategory = new QPushButton(category[i].name);
        btnCategory->setStyleSheet("QPushButton { border: none; text-align: left; padding: 5px; }"
                                   "QPushButton:hover { background: #e0e0e0; }");
        ui->verticalLayout_8->addWidget(btnCategory);

        QWidget *itemsContainer = new QWidget();
        itemsContainer->setVisible(false);
        FlowLayout *layout = new FlowLayout(itemsContainer);
        for (int j = 0; j < category[i].items.size(); ++j) {
            QToolButton *btn = new QToolButton();
            btn->setText(category[i].items[j].name);
            btn->setIcon(QIcon(category[i].items[j].iconPath));
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setIconSize(QSize(16, 16));
            btn->setToolTip(category[i].items[j].name);
            btn->setMinimumWidth(100);
            btn->setMinimumHeight(30);
            btn->setStyleSheet("QToolButton {"
                               "  background-color: #868789;"
                               "  border: 1px solid #757678;"
                               "  border-radius: 4px;"
                               "  text-align: left;"
                               "  padding: 6px 12px;"
                               "  font-size: 14px;"
                               "  font-weight: bold;"
                               "  color: #FFFFFF;"
                               "}"
                               "QToolButton:hover {"
                               "  background-color: #757678;"
                               "}"
                               "QToolButton:pressed {"
                               "  background-color: #636466;"
                               "}");
            layout->addWidget(btn);

            connect(btn, &QToolButton::clicked, [=]() {
                if (exeobj->m_filePath.isEmpty()) return;
                QString filePath = exeobj->m_filePath;
                QString objType = category[i].items[j].name;

                TFile *file = TFile::Open(filePath.toLocal8Bit().constData(), "UPDATE");
                if (!file || file->IsZombie()) {
                    return;
                }

                QString objName = objType + "_" + QString::number(QDateTime::currentSecsSinceEpoch());

                if (category[i].items[j].createAction) {
                    category[i].items[j].createAction(file, objName);
                }

                file->Close();
                delete file;
            });
        }

        ui->verticalLayout_8->addWidget(itemsContainer);

        connect(btnCategory, &QPushButton::clicked, [itemsContainer]() {
            itemsContainer->setVisible(!itemsContainer->isVisible());
        });
    }

    ui->verticalLayout_8->addStretch();
}

