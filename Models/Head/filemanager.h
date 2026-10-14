#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <vector>
enum Type {
    Executable,
    Side,
    Zombies,
};

class FileManager
{
public:
    struct RootObj {
        QString fileName;
        QString filePath;
        Type type;
    };
    struct RootProj {
        QString projName;
        QString jsonPath;
        std::vector<RootObj> objList;
    };

    int ObjDataCount = 0;
    int ProjDataCount = 0;
    std::vector<RootProj> ProjectList;
    std::vector<RootObj> ObjectList;
    void AddProject(const QString& jsonFile);
    void AddObject(const QString& fileName, const QString& filePath, const Type &type, const QString& jsonFile = "");
    void DelObject(int index, const QString& jsonFile = "");
    void LoadProjectFromJson(const QString& jsonFile);
    void ShowFileFolder();
    void MoveObj(const QString oldPaths, const QString newPath);
    void CreateFolder(QString path);
    FileManager();
};

#endif // FILEMANAGER_H
