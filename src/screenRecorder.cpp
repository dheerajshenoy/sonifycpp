#include "screenRecorder.hpp"

ScreenRecorder::ScreenRecorder() noexcept
{

    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &ScreenRecorder::callback);
}

ScreenRecorder::~ScreenRecorder() noexcept
{}

void ScreenRecorder::setGraphicsView(QGraphicsView *gv) noexcept
{
    m_gv = gv;
}

void ScreenRecorder::callback() noexcept
{
    QPixmap pix = m_gv->grab();
    pix.save(QString("temp/%1.png").arg(m_counter));
    m_counter++;
    qDebug() << "CALLBACK" << m_counter;
}

void ScreenRecorder::Start() noexcept
{
    if (m_recording) return;
    m_recording = true;
    m_counter = 0;
    m_timer.start();
    qDebug() << "RECORDING STARTED";
}

void ScreenRecorder::Stop() noexcept
{
    if (m_recording)
        m_recording = false;

    m_timer.stop();

    emit finished();
}
