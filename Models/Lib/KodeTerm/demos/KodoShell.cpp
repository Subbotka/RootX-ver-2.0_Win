#include <QApplication>
#include <QAction>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <KodoTerm.hpp>

class ShellWindow : public QMainWindow
{
    Q_OBJECT

public:
    ShellWindow()
        : QMainWindow()
    {
        setWindowTitle("KodoShell");
        resize(1000, 650);

        m_tabs = new QTabWidget(this);
        m_tabs->setTabsClosable(true);
        setCentralWidget(m_tabs);

        QMenu *file = menuBar()->addMenu("&File");
        file->addAction("&New Terminal", QKeySequence::New, this, &ShellWindow::addTerminal);
        file->addAction("&Close Terminal", this, &ShellWindow::closeTerminal);
        file->addSeparator();
        file->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

        connect(m_tabs, &QTabWidget::tabCloseRequested, this, &ShellWindow::closeTerminalIndex);

        statusBar()->showMessage("KodoShell - terminal emulator (ConPTY)");

        addTerminal();
    }

private slots:
    void addTerminal()
    {
        static int count = 1;

        KodoTerm *term = new KodoTerm(m_tabs);
        term->setMinimumSize(600, 200);
        term->setProgram("cmd.exe");
        term->setArguments(QStringList() << "/K" << "chcp" << "65001");

        int idx = m_tabs->addTab(term, QString("Terminal %1").arg(count++));
        m_tabs->setCurrentIndex(idx);
        term->start();
        term->setFocus();
    }

    void closeTerminalIndex(int index)
    {
        KodoTerm *term = qobject_cast<KodoTerm *>(m_tabs->widget(index));
        if (term)
            term->kill();
        m_tabs->removeTab(index);
        if (m_tabs->count() == 0)
            addTerminal();
    }

    void closeTerminal()
    {
        closeTerminalIndex(m_tabs->currentIndex());
    }

private:
    QTabWidget *m_tabs = nullptr;
};

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("KodoShell");
    QApplication app(argc, argv);

    ShellWindow w;
    w.show();

    return app.exec();
}

#include "KodoShell.moc"