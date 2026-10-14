#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QObject>
#include <QString>
#include <QProcess>

class QWidget;

class GameLauncher : public QObject
{
    Q_OBJECT

public:
    explicit GameLauncher(QObject *parent = nullptr);
    ~GameLauncher();

    void setParentWidget(QWidget *parent);
    void launchGame();
    void stopGame();

signals:
    void gameStarted();
    void gameFinished();
    void gameError(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void cleanupProcess();
    void openBelkaImage();

    QWidget *m_parentWidget;
    QProcess *m_process;
    QString m_gamePath;
    bool m_isGameRunning;
};

#endif // GAMELAUNCHER_H
