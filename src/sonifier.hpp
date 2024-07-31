#ifndef SONIFIER_HPP
#define SONIFIER_HPP

#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QRgb>
#include <qt6/QtCore/QObject>
#include <qt6/QtCore/QVector>
#include <qt6/QtCore/QMutex>
#include <qt6/QtCore/QMutexLocker>
#include <qt6/QtCore/QThreadPool>
#include <qt6/QtConcurrent/QtConcurrent>
#include "traverse.hpp"
#include "pixelColumn.hpp"

/*
#ifdef __linux__
    #include <alsa/asoundlib.h>
#elif __APPLE__
    #include <CoreAudio/CoreAudio.h>
#elif _WIN32
    #include <windows.h>
#endif
*/

#include "mapping.hpp"
#include "utils.hpp"

class Sonifier : public QObject
{
    Q_OBJECT
public:
    Sonifier() {}
    ~Sonifier() {}
    void setParameters(QPixmap &pix, Traverse t);
    void setParameters(QPixmap &pix, Traverse t, QVector<QPointF>);
    void setSampleRate(float SR);
    void setSamples(int nsamples);
    void LeftToRight();
    void RightToLeft();
    void TopToBottom();
    void BottomToTop();
    void CircleOutwards();
    void CircleInwards();
    void Clockwise();
    void AntiClockwise();
    void PathDrawn();
    void Sonify();
    void stopSonifying(bool state);
    bool hasStopped() { return m_stop_sonifying; }

signals:
    void sonificationDone(QVector<short>);
    void sonificationProgress(int progress);

private:
    float m_SampleRate;
    int m_nsamples;
    Mapping *m_mapping = new Mapping();

    Traverse m_traverse;
    QPixmap m_pix;
    QVector<QPointF> m_pixpos;

    bool m_stop_sonifying = false;
    QThreadPool m_threadpool;
    QImage m_img;


    /*static void processImageChunk__LeftToRight(QImage &img, int startX, int endX, Mapping *, QVector<short> &, QMutex &, std::atomic<int> &);*/
    static void processImageChunk__LeftToRight(int startX, int endX, void *userData);
    static void processImageChunk__RightToLeft(int startX, int endX, void *userData);
    static void processImageChunk__TopToBottom(int startY, int endY, void *userData);
    static void processImageChunk__BottomToTop(int startY, int endY, void *userData);
    static void processImageChunk__CircleOutwards(int startRadius, int endRadius, int &lastRadius, void *userData);
    static void processImageChunk__CircleInwards(int startRadius, int endRadius, int &lastRadius, void *userData);
    static void processImageChunk__Clockwise(int startAngle, int endAngle, void *userData);
    static void processImageChunk__AntiClockwise(int startAngle, int endAngle, void *userData);

    QVector<short> m_audioData;
    std::atomic<int> m_progressCounter = 0;

    QMutex m_mutex;
};

#endif
