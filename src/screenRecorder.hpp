#ifndef SCREENRECORDER_HPP
#define SCREENRECORDER_HPP

#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtCore/QObject>
#include <qt6/QtCore/QTimer>

class ScreenRecorder : public QObject
{
    Q_OBJECT
public:
    ScreenRecorder();
    ~ScreenRecorder();
    void setGraphicsView(QGraphicsView *gv);
    void Start();
    void Stop();

signals:
    void finished();

private:
    QGraphicsView *m_gv;
    bool m_recording = false;
    QTimer m_timer;
    int m_counter = 0;

    void callback();
};

#endif
