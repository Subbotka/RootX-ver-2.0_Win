#include <QApplication>
#include <KodoTerm.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("KodoTermSimple");
    QApplication app(argc, argv);

    KodoTerm term;
    term.setWindowTitle("KodoTerm Simple");
    term.resize(900, 600);
    term.setMinimumSize(600, 200);
    term.setProgram("cmd.exe");
    term.setArguments(QStringList() << "/K" << "chcp" << "65001");
    term.start();
    term.show();
    term.setFocus();

    return app.exec();
}