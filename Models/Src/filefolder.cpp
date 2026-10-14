#include "filefolder.h"
#include <QFileDialog>

FileFolder::FileFolder() {
}

void FileFolder::ShowFileFolder(OperaFile& operaFile) {
    QString fileName = QFileDialog::getOpenFileName();
    if(fileName.isEmpty()) return;
    qDebug() << "Выбран файл:" << fileName;

    QFileInfo fileInfo(fileName);
    QString pureName = fileInfo.fileName();

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {return;}

    QString content = QString::fromUtf8(file.readAll());
    file.close();

    // Сохраняем ПОЛНЫЙ путь к файлу, а не только директорию!
    operaFile.AddFile(pureName, content, fileName);  // <-- ИСПРАВЛЕНО: передаем fileName
}

int MainWindow::base_size = 100;

void OperaFile::AddFile(const QString& fileName, const QString& content, const QString& filePath) {
    if (dataCount >= MAX_FILES) return;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        data[dataCount] = {fileName, content, filePath};
        dataCount++;
        file.close();
    }

}

void OperaFile::AddObj(const QString& objName) {

}


void OperaFile::DelFile(int index) {
    if (index < 0 || index >= dataCount) return;
    for (int i = index; i < dataCount - 1; ++i) {
        data[i] = data[i + 1];
    }
    --dataCount;
}
