#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

class Statusbar : public QWidget
{
    Q_OBJECT
public:
    Statusbar(QWidget *parent = nullptr);

    inline void setAudioProgressText(const QString &progress) noexcept
    {
        m_progress_label->setText(progress);
    }

    inline void setAudioDurationText(const QString &duration) noexcept
    {
        m_duration_label->setText(duration);
    }

    inline void setSonificationProgress(int value) noexcept
    {
        m_progress_bar->setValue(value);
    }

    inline void setTraversalModeText(const QString &mode) noexcept
    {
        m_traversal_label->setText(mode);
    }

    void setMsg(const QString &msg, const int &s) noexcept;
    void sonificationDone() noexcept;
    void sonificationStart() noexcept;

signals:
    void stopSonificationRequested();

private:
    QLabel *m_msg_label{new QLabel()};
    QProgressBar *m_progress_bar{new QProgressBar()};
    QPushButton *m_stop_sonif_btn{new QPushButton("Stop")};
    QLabel *m_progress_label{new QLabel()};
    QLabel *m_duration_label{new QLabel()};
    QLabel *m_traversal_label{new QLabel()};
};
