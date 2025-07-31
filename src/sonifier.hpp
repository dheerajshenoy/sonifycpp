#pragma once

#include "freqmap.hpp"
#include "mapping.hpp"
#include "pixelColumn.hpp"
#include "traverse.hpp"
#include "utils.hpp"

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QPixmap>
#include <QRgb>
#include <QThreadPool>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <functional>

class Sonifier : public QObject
{
    Q_OBJECT
public:
    using MapFunc = std::function<QVector<short>(const QVector<PixelColumn> &)>;

    Sonifier();
    void setParameters(const QPixmap &pix, Traverse t) noexcept;
    void setParameters(const QPixmap &pix, Traverse t,
                       const QVector<QPointF> &) noexcept;
    void setSampleRate(float SR) noexcept;
    void setSamples(int nsamples) noexcept;
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
    inline bool hasStopped()
    {
        return m_stop_sonifying;
    }
    void setMinMax(int min, int max) noexcept;
    void setFreqMap(FreqMap f) noexcept;
    inline void setMapFunc(const MapFunc &mapFunc) noexcept
    {
        m_mapFunc = mapFunc;
    }
    inline Mapping *mapping() noexcept
    {
        return m_mapping;
    }

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

    QVector<short> m_audioData;
    std::atomic<int> m_progressCounter = 0;

    MapFunc m_mapFunc;

    QMutex m_mutex;
};
