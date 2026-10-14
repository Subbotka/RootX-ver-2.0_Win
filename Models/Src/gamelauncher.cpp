#include "Models/Head/gamelauncher.h"
#include <QFileInfo>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

GameLauncher::GameLauncher(QObject *parent)
    : QObject(parent)
    , m_parentWidget(nullptr)
    , m_process(nullptr)
    , m_isGameRunning(false)
{
    m_gamePath = "/home/baldah/Рабочий стол/RootX/RootXSquirrel/RootXBelka.x86_64";
}

GameLauncher::~GameLauncher()
{
    cleanupProcess();
}

void GameLauncher::setParentWidget(QWidget *parent)
{
    m_parentWidget = parent;
}

void GameLauncher::cleanupProcess()
{
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->terminate();
        m_process->waitForFinished(3000);
        if (m_process->state() == QProcess::Running) {
            m_process->kill();
        }
    }

    if (m_process) {
        delete m_process;
        m_process = nullptr;
    }

    m_isGameRunning = false;
}

void GameLauncher::stopGame()
{
    cleanupProcess();
}

void GameLauncher::openBelkaImage()
{
    QString imagePath = "/home/baldah/Рабочий стол/RootX/RootXSquirrel/belka.png";

    QFileInfo fileInfo(imagePath);
    if (fileInfo.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(imagePath));
    }
}

void GameLauncher::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    m_isGameRunning = false;
    emit gameFinished();
    cleanupProcess();
}

void GameLauncher::launchGame()
{
    if (m_isGameRunning || (m_process && m_process->state() == QProcess::Running)) {
        return;
    }

    cleanupProcess();

    QFileInfo fileInfo(m_gamePath);
    if (!fileInfo.exists()) {
        QString error = QString("Исполняемый файл не найден:\n%1").arg(m_gamePath);
        qWarning() << error;
        emit gameError(error);
        return;
    }

#ifdef Q_OS_LINUX
    if (!fileInfo.isExecutable()) {
        QString error = QString("Файл не является исполняемым:\n%1").arg(m_gamePath);
        qWarning() << error;
        emit gameError(error);
        return;
    }
#endif

    openBelkaImage();

    m_process = new QProcess(this);
    connect(m_process, &QProcess::finished, this, &GameLauncher::onProcessFinished);

    QString workingDir = fileInfo.absolutePath();

    m_process->setWorkingDirectory(workingDir);
    m_process->start(m_gamePath, QStringList());

    if (m_process->waitForStarted(3000)) {
        m_isGameRunning = true;
        emit gameStarted();
    } else {
        QString error = "Не удалось запустить игру: " + m_process->errorString();
        qWarning() << error;
        emit gameError(error);

        delete m_process;
        m_process = nullptr;
    }
}
