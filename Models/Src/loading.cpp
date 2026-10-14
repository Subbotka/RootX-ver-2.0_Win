#include "Models/Head/loading.h"
#include "ui_loading.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QSettings>
#include <QMessageBox>

Loading::Loading(MainWindow *mainwindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Loading)
    , m_mainwindow(mainwindow)
    , isBackgroundVisible(false)
{
    ui->setupUi(this);
    this->setWindowTitle("Запуск: RootX");
}

Loading::~Loading()
{
    delete ui;
}
void Loading::Draw(int timer, bool isFirst) {
    this->show();
    this->raise();
    QTimer::singleShot(1000 * timer, this, [this, isFirst]() {
        if (m_mainwindow) {
            m_mainwindow->setMinimumSize(0, 0);
            m_mainwindow->setMaximumSize(16777215, 16777215); // QWIDGETSIZE_MAX
            m_mainwindow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        //    m_mainwindow->show();
        }
        auto *effect = new QGraphicsOpacityEffect(this);
        effect->setOpacity(1.0);
        this->setGraphicsEffect(effect);
        auto *anim = new QPropertyAnimation(effect, "opacity", effect);
        anim->setDuration(800);              // Время исчезновения (мс)
        anim->setStartValue(1.0);            // От видимого
        anim->setEndValue(0.0);              // До прозрачного
        anim->setEasingCurve(QEasingCurve::OutQuad);

        connect(anim, &QPropertyAnimation::finished, this, [this, isFirst]() {
            if (isFirst) {
                QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DataTreeCorporation", "RootX");
                settings.setValue("first_run", false);
                settings.sync();
                if (this->graphicsEffect())  this->setGraphicsEffect(nullptr);
                DrawStage(Welcomm);
            } else {
                m_mainwindow->show();
                this->hide();
                this->setGraphicsEffect(nullptr);
                this->deleteLater();
            }
        });
        anim->start();

    });

}
#include <QWebEngineView>
void Loading::DrawStage(Stage s) {
    if (ui->widget->isVisible()) ui->widget->close();
    this->setStyleSheet("background-color: rgb(255, 245, 204);");
    this->show();

    QWidget *oldBackground = this->findChild<QWidget*>("mainWidget");
    if (oldBackground) {
        QWebEngineView *oldView = oldBackground->findChild<QWebEngineView*>();
        if (oldView) {
            oldView->setPage(nullptr);
            delete oldView;
        }
        delete oldBackground;
    }


    if (this->layout()) {
        delete this->layout();
    }

    bgLabel = new QLabel(this);
    bgLabel->setGeometry(this->rect());
    bgLabel->setScaledContents(true);
    bgLabel->lower();
    bgLabel->show();

    QHBoxLayout *l_background = new QHBoxLayout(this);
    l_background->setContentsMargins(0, 20, 0, 100);
    l_background->addStretch(1);

    QWidget *main = new QWidget(this);
    l_background->addWidget(main, 2);
    main->setObjectName("mainWidget");
    l_background->addStretch(1);

    QVBoxLayout *l_m = new QVBoxLayout(main);
    l_m->setContentsMargins(0, 0, 0, 0);
    l_m->setSpacing(10);

    QWidget *w = new QWidget(main);
    w->setStyleSheet("background-color: rgb(255, 250, 237); border-radius: 8px;");
    QVBoxLayout *l_w = new QVBoxLayout(w);
    l_w->setContentsMargins(15, 15, 15, 15);

    l_m->addWidget(w, 1);
    QWebEngineView *webEngine = new QWebEngineView(w);
    webEngine->setContextMenuPolicy(Qt::NoContextMenu);
    QUrl WebUrl;
    l_w->addWidget(webEngine, 1);

    QHBoxLayout *l_buttons = new QHBoxLayout();
    l_buttons->setSpacing(10);

    QPushButton *btnBack = new QPushButton("Назад", w);
    QPushButton *btnSkip = new QPushButton("Пропустить", w);
    QPushButton *btnNext = new QPushButton("Далее", w);
    QString buttonStyle =
        "QPushButton {"
        "  background-color: #868789;"
        "  border: 1px solid #757678;"
        "  border-radius: 4px;"
        "  text-align: center;"
        "  padding: 6px 12px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: #FFFFFF;"
        "  min-height: 28px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #757678;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #636466;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #CCCCCC;"
        "  color: #888888;"
        "  border: 1px solid #BBBBBB;"
        "}";

    btnBack->setStyleSheet(buttonStyle);
    btnNext->setStyleSheet(buttonStyle);
    btnSkip->setStyleSheet(buttonStyle);



    connect(btnBack, &QPushButton::clicked, this, [this, s]() {
        if (static_cast<int>(s) - 1 < static_cast<int>(Welcomm)) return;

        QWebEngineView *view = this->findChild<QWebEngineView*>();
        if (view) {
            view->setPage(nullptr);
            delete view;
        }

        DrawStage(static_cast<Stage>(s - 1));
    });

    connect(btnNext, &QPushButton::clicked, this, [this, s]() {
        if (static_cast<int>(s) + 1 > static_cast<int>(Settings)) {
            QWebEngineView *view = this->findChild<QWebEngineView*>();
            if (view) {
                view->setPage(nullptr);
                delete view;
            }
            this->hide();
            if (m_mainwindow) {
                m_mainwindow->show();
                m_mainwindow->raise();
                m_mainwindow->activateWindow();
            }
            this->setGraphicsEffect(nullptr);
            this->deleteLater();
        }
        else {
            QWebEngineView *view = this->findChild<QWebEngineView*>();
            if (view) {
                view->setPage(nullptr);
                delete view;
            }
            DrawStage(static_cast<Stage>(s + 1));
        }
    });

    connect(btnSkip, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите пропустить?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QWebEngineView *view = this->findChild<QWebEngineView*>();
            if (view) {
                view->setPage(nullptr);
                delete view;
            }
            this->hide();
            if (m_mainwindow) {
                m_mainwindow->show();
                m_mainwindow->raise();
                m_mainwindow->activateWindow();
            }
            this->setGraphicsEffect(nullptr);
            this->deleteLater();
        }
    });


    if (s == Welcomm) {
        WebUrl = QUrl("qrc:/source/webside/welcomm.html");
    }
    else if(s == Policy) {
        WebUrl = QUrl("qrc:/source/webside/policy.html");
    }
    else if (s == Wiki) {
        WebUrl = QUrl("qrc:/source/webside/wiki.html");

    }
    else {
        btnNext->setText("Завершить");
    }
    webEngine->setUrl(WebUrl);
    connect(webEngine, &QWebEngineView::urlChanged, this, [webEngine, WebUrl](const QUrl &url) {
        if (url != WebUrl && url.host() != WebUrl.host()) {
            webEngine->setUrl(WebUrl);
        }
    });

    l_buttons->addWidget(btnBack);
    l_buttons->addWidget(btnSkip);
    l_buttons->addWidget(btnNext);
    l_w->addLayout(l_buttons);
    main->show();
    QResizeEvent resizeEvent(this->size(), this->size());
    QApplication::sendEvent(this, &resizeEvent);
}

#include <QPainter>
void Loading::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if (bgLabel) {
        bgLabel->setGeometry(this->rect());
        QPixmap pixmap(":/source/img/fortest.png");
        if (pixmap.isNull()) return;
        QPixmap scaledPixmap = pixmap.scaledToHeight(this->height(), Qt::SmoothTransformation);
        QPixmap finalBg(this->size());
        finalBg.fill(QColor(255, 245, 204));
        QPainter p(&finalBg);
        p.drawPixmap(this->width() - scaledPixmap.width(), 0, scaledPixmap);
        p.end();
        bgLabel->setPixmap(finalBg);
    }
}
