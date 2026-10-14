#include "Models/Head/mainwindow.h"
#include "Models/Head/loading.h"
#include <QApplication>
#include "TApplication.h"
#include <QSplashScreen>
#include <QSettings>

int main(int argc, char *argv[])
{
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--disable-gpu-program-cache "
            "--disable-smooth-scrolling "
            "--js-flags='--max-old-space-size=128'");
    QCoreApplication::setOrganizationName("DataTreeCorporation");
    QCoreApplication::setApplicationName("RootX");
    QApplication qtApp(argc, argv);
    Q_INIT_RESOURCE(KodoTermThemes);

    int root_argc = 2;
    char* root_argv[] = { argv[0], (char*)"-b", nullptr };
    TApplication *app = new TApplication("uno", &root_argc, root_argv);
    MainWindow w;

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    bool IsFirstStart = settings.value("first_run", true).toBool();
    if (IsFirstStart) {
        settings.setValue("first_run", false);
        settings.sync();
    }

    Loading *loadingScreen = new Loading(&w, nullptr);
    loadingScreen->Draw(3, IsFirstStart);

    return qtApp.exec();
}