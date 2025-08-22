#pragma once

#include "freqmap.hpp"
#include "sonifycpp/MapTemplate.hpp"
#include "sonifycpp/pixel.hpp"
#include "traverse.hpp"

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QPixmap>
#include <QRgb>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <functional>

class Sonifier : public QObject
{
    Q_OBJECT
public:

    using MapFunc =
        std::function<std::vector<short>(const std::vector<Pixel> &)>;

    Sonifier(QObject *parent = nullptr);
    void setParameters(const QPixmap &pix, Traverse t) noexcept;
    void setParameters(const QPixmap &pix, Traverse t,
                       const std::vector<QPointF> &) noexcept;
    void LeftToRight() noexcept;
    void RightToLeft() noexcept;
    void TopToBottom() noexcept;
    void BottomToTop() noexcept;
    void CircleOutwards() noexcept;
    void CircleInwards() noexcept;
    void Clockwise() noexcept;
    void AntiClockwise() noexcept;
    void PathDrawn() noexcept;
    void Sonify() noexcept;
    void stopSonifying(bool state) noexcept;
    inline bool hasStopped() { return m_stop_sonifying; }
    inline void setMapFunc(MapFunc mapFunc) noexcept { m_mapFunc = mapFunc; }
    inline void setNumSamples(int n) noexcept { m_nsamples = n; }

signals:
    void sonificationDone(std::vector<short>);
    void sonificationProgress(int progress);

private:

    int m_nsamples;

    Traverse m_traverse;
    QPixmap m_pix;
    std::vector<QPointF> m_pixpos;

    bool m_stop_sonifying = false;
    QThreadPool m_threadpool;
    QImage m_img;

    static void processImageChunk__LeftToRight(int startX, int endX,
                                               void *userData) noexcept;
    static void processImageChunk__RightToLeft(int startX, int endX,
                                               void *userData) noexcept;
    static void processImageChunk__TopToBottom(int startY, int endY,
                                               void *userData) noexcept;
    static void processImageChunk__BottomToTop(int startY, int endY,
                                               void *userData) noexcept;
    static void processImageChunk__CircleOutwards(int startRadius,
                                                  int endRadius,
                                                  int &lastRadius,
                                                  void *userData) noexcept;
    static void processImageChunk__CircleInwards(int startRadius, int endRadius,
                                                 int &lastRadius,
                                                 void *userData) noexcept;
    static void processImageChunk__Clockwise(int startAngle, int endAngle,
                                             void *userData) noexcept;
    static void processImageChunk__AntiClockwise(int startAngle, int endAngle,
                                                 void *userData) noexcept;

    std::vector<short> m_audioData;
    std::atomic<int> m_progressCounter = 0;

    MapFunc m_mapFunc;

    QMutex m_mutex;
};
