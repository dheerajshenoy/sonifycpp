#pragma once

#include <QPixmap>
#include <QRgb>
#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include "traverse.hpp"
#include "pixelColumn.hpp"
#include "freqmap.hpp"
#include "mapping.hpp"
#include "utils.hpp"

class Sonifier : public QObject
{
    Q_OBJECT
public:
    Sonifier() = default;
    void setParameters(const QPixmap& pix, const Traverse& t) noexcept;
    void setParameters(const QPixmap& pix, const Traverse& t, const QVector<QPointF>&) noexcept;
    void setSampleRate(const float& SR) noexcept;
    void setSamples(const int& nsamples) noexcept;
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
    void stopSonifying(const bool& state) noexcept;
    inline bool hasStopped() { return m_stop_sonifying; }
    void setMinMax(const int &min, const int &max) noexcept;
    void setFreqMap(const FreqMap &f) noexcept;

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

    static void processImageChunk__LeftToRight(int startX, int endX, void *userData) noexcept;
    static void processImageChunk__RightToLeft(int startX, int endX, void *userData) noexcept;
    static void processImageChunk__TopToBottom(int startY, int endY, void *userData) noexcept;
    static void processImageChunk__BottomToTop(int startY, int endY, void *userData) noexcept;
    static void processImageChunk__CircleOutwards(int startRadius, int endRadius, int &lastRadius, void *userData) noexcept;
    static void processImageChunk__CircleInwards(int startRadius, int endRadius, int &lastRadius, void *userData) noexcept;
    static void processImageChunk__Clockwise(int startAngle, int endAngle, void *userData) noexcept;
    static void processImageChunk__AntiClockwise(int startAngle, int endAngle, void *userData) noexcept;

    QVector<short> m_audioData;
    std::atomic<int> m_progressCounter = 0;

    QMutex m_mutex;
};
