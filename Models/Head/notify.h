#ifndef NOTIFY_H
#define NOTIFY_H

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>

class Notify : public QWidget
{
    Q_OBJECT

public:
    Notify(QWidget *parent = nullptr);
    void Show(const QString &title, const QString &maintext, int time, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateProgress();
    void updateProgressBarWidth();
    void hideNotification();
    void copyToClipboard();

private:
    QLabel *m_titleLabel;
    QLabel *m_textLabel;
    QLabel *m_iconLabel;
    QWidget *m_progressBar;
    QPushButton *m_copyButton;

    int m_duration;
    int m_remaining;
    QTimer *m_timer;

    int m_posX;
    int m_posY;
};

#endif
