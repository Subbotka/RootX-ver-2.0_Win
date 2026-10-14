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
#include <QSignalBlocker>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QPalette>
#include <QPointer>
#include <QPainter>
#include <QPaintEvent>
#include <QTreeWidget>
#include <QAbstractItemView>
#include <QTreeWidgetItem>
#include <functional>
#include <utility>
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

namespace {

class PropertyNameEdit : public QLineEdit
{
public:
    PropertyNameEdit(const QString &text, QAbstractItemView *view, QWidget *parent)
        : QLineEdit(text, parent), m_view(view)
    {
        setFrame(false);
        setAutoFillBackground(true);
        setFocusPolicy(Qt::StrongFocus);
        blankUnderlyingText(text);
    }

    std::function<void()> onCommitRequest;
    std::function<void()> onCancelRequest;

    void blankUnderlyingText(const QString &currentText)
    {
        if (!m_view)
            return;
        auto *tree = qobject_cast<QTreeWidget *>(m_view.data());
        if (!tree)
            return;
        QTreeWidgetItem *item = tree->currentItem();
        if (!item)
            return;
        if (!m_item) {
            if (item->text(1) != currentText)
                return;
            m_item = item;
        }
        m_item->setText(1, QString());
        m_view->viewport()->update();
    }

    void restoreUnderlyingText(const QString &value)
    {
        if (!m_item)
            return;
        if (m_view) {
            m_item->setText(1, value);
            m_view->viewport()->update();
        }
        m_item = nullptr;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        {
            QPainter painter(this);
            painter.fillRect(rect(), palette().brush(QPalette::Base));
        }
        QLineEdit::paintEvent(event);
    }


    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape) {
            if (onCancelRequest) onCancelRequest();
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (onCommitRequest) onCommitRequest();
            clearFocus();
            event->accept();
            return;
        }
        QLineEdit::keyPressEvent(event);
    }

    void focusOutEvent(QFocusEvent *event) override
    {
        QLineEdit::focusOutEvent(event);
        if (onCommitRequest) onCommitRequest();
    }

private:
    QPointer<QAbstractItemView> m_view;
    QTreeWidgetItem *m_item = nullptr;
};

class DeferredVariantEditorFactory : public QtVariantEditorFactory
{
public:
    explicit DeferredVariantEditorFactory(QObject *parent = nullptr)
        : QtVariantEditorFactory(parent)
    {
    }

    void configure(QtProperty *property, std::function<void(QtProperty *, const QVariant &)> handler)
    {
        m_property = property;
        m_handler = std::move(handler);
    }

protected:
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent) override
    {
        if (!manager || property != m_property)
            return QtVariantEditorFactory::createEditor(manager, property, parent);

        auto *view = qobject_cast<QAbstractItemView *>(parent ? parent->parentWidget() : nullptr);
        const QString value = manager->value(property).toString();
        auto *edit = new PropertyNameEdit(value, view, parent);
        edit->setModified(false);

        const auto refresh = [manager, property, edit]() {
            const QString current = manager->value(property).toString();
            if (edit->hasFocus() && edit->isModified()) {
                edit->blankUnderlyingText(current);
                return;
            }
            edit->setText(current);
            edit->setModified(false);
        };

        edit->onCommitRequest = [this, manager, property, edit]() {
            const QString text = edit->text();
            if (manager->value(property).toString() == text) {
                edit->setModified(false);
                edit->restoreUnderlyingText(manager->value(property).toString());
                return;
            }
            if (m_handler)
                m_handler(property, text);
            const QString current = manager->value(property).toString();
            edit->setText(current);
            edit->setModified(false);
            edit->restoreUnderlyingText(current);
        };

        edit->onCancelRequest = [manager, property, edit]() {
            const QString current = manager->value(property).toString();
            edit->setText(current);
            edit->setModified(false);
            edit->restoreUnderlyingText(current);
            edit->clearFocus();
        };

        connect(manager, &QtVariantPropertyManager::valueChanged, edit, refresh);
        connect(manager, &QtAbstractPropertyManager::propertyChanged, edit, refresh);

        return edit;
    }

    void connectPropertyManager(QtVariantPropertyManager *manager) override
    {
        QtVariantEditorFactory::connectPropertyManager(manager);
    }

    void disconnectPropertyManager(QtVariantPropertyManager *manager) override
    {
        QtVariantEditorFactory::disconnectPropertyManager(manager);
    }

private:
    QtProperty *m_property = nullptr;
    std::function<void(QtProperty *, const QVariant &)> m_handler;
};

}

void Editor::setupEditor(TObject *robj)
{
    qDebug() << "[PROP] setupEditor begin";
    if (!robj) return;

    if (editorPropety) {
        if (QLineEdit *activeEdit = editorPropety->findChild<QLineEdit *>()) {
            if (activeEdit->hasFocus())
                activeEdit->clearFocus();
        }
    }

    // 1. Чистим старый редактор и ВСЕ связанные с ним ресурсы
    if (editorPropety) {
        editorPropety->deleteLater();
        editorPropety = nullptr;
    }
    if (m_variantManager) {
        disconnect(m_variantManager, nullptr, this, nullptr); // Отключаем старые сигналы
        m_variantManager->deleteLater();
        m_variantManager = nullptr;
    }
    if (m_variantFactory) {
        m_variantFactory->deleteLater(); // ИСПРАВЛЕНО: удаляем старую фабрику, чтобы не копилась память
        m_variantFactory = nullptr;
    }


    // 2. Создаем менедер и фабрику заново
    m_variantManager = new QtVariantPropertyManager(this);
    auto *nameFactory = new DeferredVariantEditorFactory(this);
    m_variantFactory = nameFactory;

    // 3. Создаем контейнер и разметку
    editorPropety = new QWidget(this);
    QVBoxLayout *verticalLay = new QVBoxLayout();
    verticalLay->setContentsMargins(0, 0, 0, 0); // Убираем лишние рамки для красоты в Dock
    editorPropety->setLayout(verticalLay);


    QtTreePropertyBrowser *propertyBrowser = new QtTreePropertyBrowser(editorPropety);
    propertyBrowser->setFactoryForManager(m_variantManager, m_variantFactory);
    propertyBrowser->setPropertiesWithoutValueMarked(true); // Чтобы группы выделялись жирным
    verticalLay->addWidget(propertyBrowser);

    mainProp = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), "Основное");
    m_nameObjProp = m_variantManager->addProperty(QVariant::String, "Название объекта");
    {
        const QSignalBlocker blocker(m_variantManager);
        m_nameObjProp->setValue(QString::fromStdString(robj->GetName()));
    }
    nameFactory->configure(m_nameObjProp, [this](QtProperty *p, const QVariant &v) { onPropertyValueChanged(p, v); });

    mainProp->addSubProperty(m_nameObjProp);

    propertyBrowser->addProperty(mainProp);




    if (!m_editorDock) {
        m_editorDock = new ads::CDockWidget("Редактировать");
        m_editorDock->setObjectName("editorDock");
        m_editorDock->setWidget(editorPropety);
        m_editorDock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
        ads::CDockWidget *panelDock = m_dockManager->findDockWidget("panelDock");
        ads::CDockAreaWidget *panelArea = panelDock ? panelDock->dockAreaWidget() : nullptr;
        if (panelArea)
            m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_editorDock, panelArea);
        else
            m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_editorDock);
    } else {
        m_editorDock->setWidget(editorPropety);
    }
    qDebug() << "[PROP] setupEditor end";
}

void Editor::onPropertyValueChanged(QtProperty *property, const QVariant &value)
{
    if (property != m_nameObjProp)
        return;

    if (!exeobj || !drawobj) {
        qWarning() << "[PROP] нет файла или объекта для переименования";
        return;
    }

    const auto syncModel = [this](const QString &name) {
        if (!m_variantManager || !m_nameObjProp)
            return;
        const QSignalBlocker blocker(m_variantManager);
        m_nameObjProp->setValue(name);
    };

    TNamed *namedDrawObj = dynamic_cast<TNamed*>(drawobj);
    if (!namedDrawObj) {
        qWarning() << "[PROP] объект не является TNamed:" << drawobj->ClassName();
        return;
    }

    const QString oldName = QString::fromUtf8(namedDrawObj->GetName());
    const QString newName = value.toString().trimmed();

    if (newName.isEmpty()) {
        qWarning() << "[PROP] имя объекта не может быть пустым";
        syncModel(oldName);
        return;
    }

    static const QString forbidden = QStringLiteral("/*;");
    if (newName.contains(forbidden) || newName.contains(QStringLiteral(" ")) || newName.contains(QStringLiteral("\t"))) {
        qWarning() << "[PROP] недопустимое имя:" << newName;
        syncModel(oldName);
        return;
    }

    if (oldName == newName) {
        syncModel(oldName);
        return;
    }

    if (m_openFile) {
        m_openFile->Close();
        delete m_openFile;
        m_openFile = nullptr;
    }

    TFile file(exeobj->m_filePath.toUtf8().constData(), "update");
    if (file.IsZombie()) {
        qWarning() << "[PROP] не удалось открыть файл на запись:" << exeobj->m_filePath;
        syncModel(oldName);
        return;
    }

    const QByteArray oldKeyName = oldName.toUtf8();
    const QByteArray newKeyName = newName.toUtf8();

    if (!file.GetKey(oldKeyName.constData())) {
        qWarning() << "[PROP] объект не найден в файле:" << oldName;
        file.Close();
        syncModel(oldName);
        return;
    }

    if (file.GetKey(newKeyName.constData())) {
        qWarning() << "[PROP] объект с таким именем уже существует:" << newName;
        file.Close();
        syncModel(oldName);
        return;
    }

    TObject *fileObj = file.Get(oldKeyName.constData());
    if (!fileObj) {
        qWarning() << "[PROP] не удалось прочитать объект из файла:" << oldName;
        file.Close();
        syncModel(oldName);
        return;
    }

    TNamed *fileNamed = dynamic_cast<TNamed*>(fileObj);
    if (!fileNamed) {
        qWarning() << "[PROP] объект в файле не является TNamed:" << fileObj->ClassName();
        file.Close();
        syncModel(oldName);
        return;
    }

    fileNamed->SetName(newKeyName.constData());

    if (QString::fromUtf8(fileObj->GetName()) != newName) {
        qWarning() << "[PROP] ROOT отклонил новое имя:" << newName;
        file.Close();
        syncModel(oldName);
        return;
    }

    const Int_t written = file.WriteTObject(fileObj, newKeyName.constData(), "overwrite");
    if (written <= 0) {
        fileNamed->SetName(oldKeyName.constData());
        qWarning() << "[PROP] ROOT не смог записать объект с новым именем";
        file.Close();
        syncModel(oldName);
        return;
    }

    const QByteArray oldKeyWithCycles = (oldName + QStringLiteral(";*")).toUtf8();
    file.Delete(oldKeyWithCycles.constData());
    file.Close();

    namedDrawObj->SetName(newKeyName.constData());
    syncModel(newName);

    if (m_currentTreeItem) {
        const quintptr storedObj = m_currentTreeItem->data(0, Qt::UserRole).value<quintptr>();
        if (storedObj == reinterpret_cast<quintptr>(drawobj))
            m_currentTreeItem->setText(0, newName);
    }

    qDebug() << "[PROP] имя изменено:" << oldName << "->" << newName;
    (new Notify())->Show("Переименование", QString("Объект переименован:\n%1 -> %2").arg(oldName, newName), 4, this);
}



