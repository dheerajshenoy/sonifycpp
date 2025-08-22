#include "statusbar.hpp"

#include <QHBoxLayout>
#include <QTimer>

Statusbar::Statusbar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_msg_label);
    layout->addWidget(m_progress_bar);
    layout->addStretch(1);

    m_progress_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout->addWidget(m_traversal_label);
    layout->addWidget(m_stop_sonif_btn);
    layout->addWidget(m_progress_label);
    layout->addWidget(m_duration_label);

    connect(m_stop_sonif_btn, &QPushButton::clicked, this,
            [&]() { emit stopSonificationRequested(); });

    m_stop_sonif_btn->setVisible(false);
    m_progress_bar->setVisible(false);

    this->setLayout(layout);
}

// This is used to show statusbar message for specified seconds `s` or
// indefinetely if set equal to -1
void
Statusbar::setMsg(const QString &msg, const int &s) noexcept
{
    if (msg.isEmpty()) return;

    if (s > 0)
    {
        m_msg_label->setText(msg);
        QTimer::singleShot(s * 1000, [&]() { m_msg_label->clear(); });
    }
    else { m_msg_label->setText(msg); }
}

void
Statusbar::sonificationDone() noexcept
{
    m_progress_bar->setVisible(false);
    m_stop_sonif_btn->setVisible(false);
}

void
Statusbar::sonificationStart() noexcept
{
    m_progress_bar->setVisible(true);
    m_progress_bar->reset();
    m_stop_sonif_btn->setVisible(true);
}

void
Statusbar::reset() noexcept
{
    m_progress_label->clear();
    m_traversal_label->clear();
    m_duration_label->clear();
    m_msg_label->clear();
}
