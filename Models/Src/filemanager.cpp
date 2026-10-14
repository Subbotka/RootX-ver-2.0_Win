#include "Models/Head/filemanager.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>

FileManager::FileManager() {}

void FileManager::ShowFileFolder() {
    QString filter = "Файлы аналитики (*.root *.json);;"
                     "ROOT файлы (*.root);;"
                     "JSON файлы (*.json);;"
                     "CSV файлы (*.csv)";

    QString fileName = QFileDialog::getOpenFileName(
        nullptr,
        "Открыть файл проекта или данных",
        "",
        filter
        );

    if (fileName.isEmpty()) return;
    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix().toLower();
    QString pureName = fileInfo.fileName();

    qDebug() << "Выбран файл:" << pureName << "Расширение:" << extension;
    if (extension == "root") {
        qDebug() << "Логика для бинарного ROOT-файла. Читаем структуру TTree...";
        AddObject(pureName, fileName, Executable);
    }
    else if (extension == "json") {
        qDebug() << "Логика для текстового JSON-файла конфигурации...";
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть JSON файл для чтения";
            return;
        }

        QString content = QString::fromUtf8(file.readAll());
        file.close();
       LoadProjectFromJson(fileName);
    }
    else if (extension == "csv") {
        AddObject(pureName, fileName, Side);
    }
    else {
        qWarning() << "Критическая ошибка: Выбран неподдерживаемый формат файла!";
        return;
    }
}

void FileManager::AddObject(const QString &fileName, const QString &filePath, const Type &type, const QString &jsonFile) {
    if (jsonFile.isEmpty()) {
        qDebug() << "Объект который пытается добавить пользователь не имеет родителя (проект)";
        RootObj obj = {fileName, filePath, type};
        ObjectList.push_back(obj);
        ObjDataCount+=1;
        return;
    }
}
void FileManager::DelObject(int index, const QString &jsonFile) {
    if (jsonFile.isEmpty()) {
        qDebug() << "Объект который пытаются удалить не имеет родителя (проект)";
        ObjectList.erase(ObjectList.begin() + index);
        ObjDataCount-=1;
    }
}
void FileManager::LoadProjectFromJson(const QString& jsonFile) {
    QFile file(jsonFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл проекта:" << jsonFile;
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject rootObject = jsonDoc.object();
    QString projName = rootObject["ProjectName"].toString();
    QString localProjectPath = rootObject["ProjectPath"].toString();
    if (localProjectPath.endsWith("/")) {
        localProjectPath.chop(1);
    }
    RootProj newProject;
    newProject.projName = projName;
    newProject.jsonPath = jsonFile;
    QJsonArray objectsArray = rootObject["Objects"].toArray();
    for (int i = 0; i < objectsArray.size(); ++i) {
        QString fileNameStr = objectsArray[i].toString();
        RootObj newObj;
        newObj.fileName = fileNameStr;
        newObj.filePath = localProjectPath + "/" + fileNameStr;

        QString extension = newObj.fileName.split('.').last().toLower();
        if (extension == "root" ) newObj.type = Executable;
        else if (extension == "csv") newObj.type = Side;
        else newObj.type = Zombies;
        newProject.objList.push_back(newObj);

        qDebug() << "В проект" << projName << "подтянут файл:" << newObj.fileName;
    }
    ProjectList.push_back(newProject);
    ProjDataCount+=1;

    qDebug() << "Успешно загружен проект:" << projName << "| Всего проектов в памяти:" << ProjDataCount;
}
void moveFile(const QString &oldPath, const QString &newFolder) {
    if (QFile::rename(oldPath, newFolder)) {
        qDebug() << "Файл успешно переброшен!";
    } else {
        qDebug() << "Ошибка переноса. Проверьте пути, или не открыт ли файл.";
    }
}

void FileManager::CreateFolder(QString path) {
    QFileInfo info(path);
    QString fileName = info.fileName();
    if (!fileName.isEmpty() && fileName.contains('.')) {
        path = QDir::cleanPath(info.absolutePath() + "/" + info.baseName());
    } else {
        path = QDir::cleanPath(path);
    }
    QDir dir;
    if (!dir.exists(path)) {
        if (dir.mkpath(path)) {
            qDebug() << "Папка успешно создана:" << path; // Создаст /home/.../Документы/titanc
        } else {
            qDebug() << "Не удалось создать папку:" << path;
        }
    } else {
        qDebug() << "Папка уже существует:" << path;
    }
}





