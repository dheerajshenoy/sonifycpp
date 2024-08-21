#pragma once

#include <QGraphicsView>
#include <QObject>
#include <QTimer>

class ScreenRecorder : public QObject
{
    Q_OBJECT
public:
    ScreenRecorder() noexcept;
    ~ScreenRecorder() noexcept;
    void setGraphicsView(QGraphicsView *gv) noexcept;
    void Start() noexcept;
    void Stop() noexcept;

signals:
    void finished();

private:
    void callback() noexcept;

    QGraphicsView *m_gv = nullptr;
    bool m_recording = false;
    QTimer m_timer;
    int m_counter = 0;

};
