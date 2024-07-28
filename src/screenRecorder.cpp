#include "screenRecorder.hpp"

ScreenRecorder::ScreenRecorder()
{

    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &ScreenRecorder::callback);
}

ScreenRecorder::~ScreenRecorder()
{}

void ScreenRecorder::setGraphicsView(QGraphicsView *gv)
{
    m_gv = gv;
}

void ScreenRecorder::callback()
{
    QPixmap pix = m_gv->grab();
    pix.save(QString("temp/%1.png").arg(m_counter));
    m_counter++;
    qDebug() << "CALLBACK" << m_counter;
}

void ScreenRecorder::Start()
{
    if (m_recording) return;
    m_recording = true;
    m_counter = 0;
    m_timer.start();
    qDebug() << "RECORDING STARTED";
}

void ScreenRecorder::Stop()
{
    if (m_recording)
        m_recording = false;

    m_timer.stop();

    emit finished();
}
