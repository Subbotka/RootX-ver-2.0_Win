#include "Models/Head/editor.h"
#include "ui_editor.h"
#include "TFile.h"
#include <TFile.h>
#include "TH1.h"
#include <TKey.h>
#include <TROOT.h>
#include "Models/Head/executableobjects.h"
#include <TTree.h>
#include <QTimer>
void Editor::setupTree()
{
    if (m_openFile) {
        m_openFile->Close();
        delete m_openFile;
        m_openFile = nullptr;
    }

    disconnect(ui->treeWidget, &QTreeWidget::itemClicked, this, &Editor::onTreeItemClicked);

    ui->treeWidget->clear();
    ui->treeWidget->setObjectName("fileTreeWidget");
    ui->treeWidget->setHeaderLabels({"Имя", "Тип"});
    ui->treeWidget->setColumnWidth(0, 200);
    ui->treeWidget->setColumnWidth(1, 150);

    m_openFile = TFile::Open(exeobj->m_filePath.toLocal8Bit().constData(), "READ");
    if (!m_openFile || m_openFile->IsZombie()) {
        if (m_openFile) { delete m_openFile; m_openFile = nullptr; }
        return;
    }

    TList *keys = m_openFile->GetListOfKeys();
    if (!keys) {
        QTreeWidgetItem *errorItem = new QTreeWidgetItem(ui->treeWidget);
        errorItem->setIcon(0,QIcon(":/source/img/exclamation.png"));
        errorItem->setText(1, "");
        return;
    }

    if (keys->GetEntries() == 0) {
        QTreeWidgetItem *emptyItem = new QTreeWidgetItem(ui->treeWidget);
        emptyItem->setIcon(0,QIcon(":/source/img/exclamation.png"));
        emptyItem->setText(1, "");
        return;
    }

    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setIcon(0,QIcon(":/source/img/folder.png"));
    rootItem->setText(0, exeobj->m_namefile);
    rootItem->setText(1, "TFile");
    rootItem->setExpanded(true);

    addDirectoryItems(m_openFile, rootItem);
    ui->treeWidget->expandItem(rootItem);

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &Editor::onTreeItemClicked);
}

void Editor::addDirectoryItems(TDirectory *dir, QTreeWidgetItem *parentItem) {
    if (!dir || !parentItem) return;

    TList *keys = dir->GetListOfKeys();
    if (!keys) return;

    TIter next(keys);
    TKey *key;

    while ((key = (TKey*)next())) {
        if (!key) continue;

        TObject *obj = key->ReadObj();
        if (!obj) continue;

        if (obj->InheritsFrom("TH1")) {
            static_cast<TH1*>(obj)->SetDirectory(nullptr);
        } else if (obj->InheritsFrom("TTree")) {
            static_cast<TTree*>(obj)->SetDirectory(nullptr);
        }
        gROOT->GetList()->Add(obj);

        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);

        QString icon = ":/source/img/picture_empty.png";
        if (obj->InheritsFrom("TDirectory") || obj->InheritsFrom("TDirectoryFile")) {
            icon = ":/source/img/folder.png";
        } else if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TH2") || obj->InheritsFrom("TH3")) {
            icon = ":/source/img/chart_bar.png";
        } else if (obj->InheritsFrom("TGraph")) {
            icon = ":/source/img/chart_curve.png";
        } else if (obj->InheritsFrom("TTree") || obj->InheritsFrom("TNtuple")) {
            icon = ":/source/img/application_side_tree.png";
        } else if (obj->InheritsFrom("TCanvas")) {
            icon = ":/source/img/palette.png";
        } else if (obj->InheritsFrom("TF1")) {
            icon = ":/source/img/chart_line.png";
        }

        item->setIcon(0,QIcon(icon));
        item->setText(0, obj->GetName());
        item->setText(1, obj->ClassName());

        item->setData(0, Qt::UserRole, QVariant::fromValue((quintptr)obj));

        if (obj->InheritsFrom("TDirectory") || obj->InheritsFrom("TDirectoryFile")) {
            TDirectory *subDir = dynamic_cast<TDirectory*>(obj);
            if (subDir && subDir != dir) {
                addDirectoryItems(subDir, item);
            }
        }
        else if (obj->InheritsFrom("TTree") || obj->InheritsFrom("TNtuple")) {
            TTree *tree = dynamic_cast<TTree*>(obj);
            if (tree) {
                TObjArray *branches = tree->GetListOfBranches();
                if (branches) {
                    TIter branchIter(branches);
                    TBranch *branch;
                    while ((branch = (TBranch*)branchIter())) {
                        QTreeWidgetItem *branchItem = new QTreeWidgetItem(item);
                        branchItem->setText(0, QString("🌿 %1").arg(branch->GetName()));
                        branchItem->setText(1, "TBranch");
                        branchItem->setData(0, Qt::UserRole, QVariant::fromValue((quintptr)branch));

                        TObjArray *leafList = branch->GetListOfLeaves();
                        if (leafList) {
                            TIter leafIter(leafList);
                            TLeaf *leaf;
                            while ((leaf = (TLeaf*)leafIter())) {
                                QTreeWidgetItem *leafItem = new QTreeWidgetItem(branchItem);
                                QString typeName = leaf->GetTypeName();
                                leafItem->setText(0, QString("🍃 %1").arg(leaf->GetName()));
                                leafItem->setText(1, typeName);
                                leafItem->setData(0, Qt::UserRole, QVariant::fromValue((quintptr)leaf));
                            }
                        }
                    }
                }
            }
        }
    }
}

void Editor::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    if (!item) return;
    Q_UNUSED(column);

    m_currentTreeItem = item;

    QVariant ElemName = item->data(0, Qt::UserRole);
    if (!ElemName.isValid()) return;

    quintptr ptr = ElemName.value<quintptr>();
    TObject *obj = reinterpret_cast<TObject*>(ptr);
    if (!obj) return;

    QTimer::singleShot(0, this, [this, obj]() {
        qDebug() << "[TREE] click->setupCodeEditor";
        setupCodeEditor(obj);
        qDebug() << "[TREE] click->setupCanvas";
        setupCanvas(obj);
        qDebug() << "[TREE] click->setupEditor";
        setupEditor(obj);
        drawobj = obj;
        qDebug() << "[TREE] click done";
    });
}

