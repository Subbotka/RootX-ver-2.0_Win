#include "Models/Head/notify.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFontMetrics>
#include <QApplication>
#include <QClipboard>
#include <QGraphicsOpacityEffect>

Notify::Notify(QWidget *parent)
    : QWidget(parent)
    , m_posX(0)
    , m_posY(0)
    , m_duration(0)
    , m_remaining(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setScaledContents(true);
    QGraphicsOpacityEffect *iconOpacity = new QGraphicsOpacityEffect(m_iconLabel);
    iconOpacity->setOpacity(0.3);
    m_iconLabel->setGraphicsEffect(iconOpacity);
    m_iconLabel->lower();

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet("color: #333333; font-size: 15px; font-weight: bold;");
    m_titleLabel->raise();

    m_copyButton = new QPushButton("Копировать", this);
    m_copyButton->setStyleSheet(
        "QPushButton {"
        "  background: #f0f0f0;"
        "  color: #333333;"
        "  border: 1px solid #c8c8c8;"
        "  border-radius: 4px;"
        "  font-size: 11px;"
        "  padding: 3px 10px;"
        "}"
        "QPushButton:hover { background: #e2e2e2; }"
        "QPushButton:pressed { background: #d5d5d5; }");
    m_copyButton->raise();
    connect(m_copyButton, &QPushButton::clicked, this, &Notify::copyToClipboard);

    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet("color: #555555; font-size: 13px;");
    m_textLabel->setWordWrap(true);
    m_textLabel->raise();

    m_progressBar = new QWidget(this);
    m_progressBar->setFixedHeight(3);
    m_progressBar->setStyleSheet("background: #888888; border-radius: 1px;");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Notify::updateProgress);

    hide();
}

void Notify::Show(const QString &title, const QString &maintext, int time, QWidget *parent)
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_titleLabel->setText(title);
    m_textLabel->setText(maintext);

    // Размер подстраивается под текст: фиксированная ширина, высота по содержимому.
    const int W = 520;
    const int textW = W / 2 - 20;
    QFontMetrics fm = m_textLabel->fontMetrics();
    int textH = fm.boundingRect(QRect(0, 0, textW, 10000),
                                Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                                maintext).height();
    if (textH < 50) textH = 50;
    if (textH > 600) textH = 600;
    setFixedSize(W, 43 + textH + 15);

    m_duration = time * 1000;
    m_remaining = m_duration;
    if (parent) {
        QPoint globalPos = parent->mapToGlobal(QPoint(0, qMax(0, parent->height() - height())));
        move(globalPos);
    }
    updateProgressBarWidth();
    show();
    raise();
    activateWindow();

    m_timer->start(50);
}

void Notify::updateProgress()
{
    m_remaining -= 50;

    if (m_remaining <= 0) {
        m_timer->stop();
        hideNotification();
        return;
    }

    updateProgressBarWidth();
}

void Notify::updateProgressBarWidth()
{
    if (m_duration <= 0) return;

    double progress = (double)m_remaining / m_duration;
    int barWidth = (int)((width() - 24) * progress);
    m_progressBar->setFixedWidth(barWidth);
}

void Notify::hideNotification()
{
    hide();
}

void Notify::copyToClipboard()
{
    QString text = m_titleLabel->text();
    const QString body = m_textLabel->text();
    if (!body.isEmpty()) text += "\n" + body;

    QApplication::clipboard()->setText(text);

    const QString prev = m_copyButton->text();
    m_copyButton->setText("Скопировано");
    QTimer::singleShot(1500, this, [this, prev]() {
        m_copyButton->setText(prev);
    });
}

void Notify::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_iconLabel->setGeometry(width() / 2, 0, width() - width() / 2, height());
    int textX = 20;
    int textW = width() / 2 - 20;
    m_titleLabel->setGeometry(textX, 9, textW - 100, 25);
    m_textLabel->setGeometry(textX, 43, textW, height() - 55);
    m_copyButton->setGeometry(width() - 100, 8, 88, 26);
    m_progressBar->setGeometry(10, height() - 8, width() - 20, 3);
}

void Notify::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    int radius = 10;

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    QColor bgColor(255, 255, 255, 240);
    painter.fillPath(path, bgColor);

    QPen borderPen(QColor(220, 220, 220, 200), 1);
    painter.setPen(borderPen);
    painter.drawPath(path);
}
