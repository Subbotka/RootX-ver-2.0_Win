#include "Models/Head/sideobjects.h"
#include "ui_sideobjects.h"
#include "Models/Lib/Spreadsheet/spreadsheet.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

SideObjects::SideObjects(QWidget *parent, MainWindow *mainwindow,  const FileManager::RootObj &obj)
    : DraggableWidget(parent)
    , ui(new Ui::SideObjects)
    , m_obj(obj)
    , m_table(nullptr)
    , m_mainwindow(mainwindow)
{

    // =================================================================================
    // БУДУЩИЙ СЦЕНАРИЙ ИСПОЛЬЗОВАНИЯ (by T.L.)
    //
    // 1. Управление данными (m_obj / Side Obj):
    //    - Набор данных передается в таблицу.
    //    - Внутри таблицы выполняется парсинг (например, для формата .csv).
    //    - НЕОБХОДИМО РЕАЛИЗОВАТЬ:
    //      * Метод для чтения/получения строк из файла и их переноса в таблицу объекта.
    //      * Событие (Event) для перехвата любых изменений данных внутри таблицы,
    //        которое будет синхронно дублировать эти изменения обратно в файл.
    //
    // 2. Управление файлами (FileManager):
    //    - Компонент необходим для совместного создания исполняемого объекта
    //      (Executable Obj) и связанного с ним проекта.
    // =================================================================================

    ui->setupUi(this);
    ui->title_label->setText(m_obj.fileName);
    m_table = new SpreadSheet(3,5, this, this);
    ui->horizontalLayout_3->addWidget(m_table);
    CSVToArray(m_obj.filePath);
}

SideObjects::~SideObjects()
{
    delete ui;
}


void SideObjects::CSVToArray(const QString &filepath) {
    QFile file(filepath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        data.clear();
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList next = line.split(',');
            data.append(next.toVector());
        }

        file.close();
    }

}
