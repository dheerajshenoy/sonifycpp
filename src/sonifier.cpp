// Contains the code for different traversal types.

#include "sonifier.hpp"

#include <concepts>
#include <qnamespace.h>

Sonifier::Sonifier(QObject *parent) : QObject(parent)
{
    m_mapFunc = [this](const std::vector<Pixel> &cols)
    { return m_mapping->Map__Intensity(cols); };
}

void
Sonifier::setSamples(int nsamples) noexcept
{
    if (nsamples <= 0) return;

    m_nsamples = nsamples;
    m_mapping->setSamples(nsamples);
}

void
Sonifier::setSampleRate(float SR) noexcept
{
    if (SR <= 0.0f) return;

    m_SampleRate = SR;
    m_mapping->setSampleRate(SR);
}

void
Sonifier::setFreqMap(FreqMap m) noexcept
{
    m_mapping->setFreqMap(m);
}

void
Sonifier::setMinMax(int min, int max) noexcept
{
    m_mapping->setMinMax(min, max);
}

void
Sonifier::processImageChunk__LeftToRight(int startX, int endX, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int imgHeight    = son->m_img.height();
    const int totalsamples = son->m_img.width();

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(imgHeight);

    for (int x = startX; x < endX && !son->m_stop_sonifying; ++x)
    {
        for (int y = 0; y < imgHeight; y++)
        {
            QRgb pixel = son->m_img.pixel(x, y);
            pixcols[y] = Pixel{ static_cast<std::uint8_t>(qRed(pixel)),
                                static_cast<std::uint8_t>(qGreen(pixel)),
                                static_cast<std::uint8_t>(qBlue(pixel)),
                                static_cast<std::uint8_t>(qAlpha(pixel)),
                                x,
                                y };
        }

        temp = son->m_mapFunc(pixcols);

        const int index = x * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__RightToLeft(int startX, int endX, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int imgHeight    = son->m_img.height();
    const int totalsamples = son->m_img.width();

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(imgHeight);

    for (int x = startX; x < endX && !son->m_stop_sonifying; x++)
    {
        for (int y = 0; y < son->m_img.height(); y++)
            pixcols[y] = Pixel{ son->m_img.pixel(x, y), x, y };

        temp = son->m_mapFunc(pixcols);

        const int index = (totalsamples - 1 - x) * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__TopToBottom(int startY, int endY, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int totalsamples = son->m_img.height();

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(totalsamples);

    for (int y = startY; y < endY && !son->m_stop_sonifying; y++)
    {
        for (int x = 0; x < son->m_img.width(); x++)
            pixcols[x] = Pixel{ son->m_img.pixel(x, y), x, y };

        temp = son->m_mapping->Map__HSV(pixcols);

        const int index = startY + y * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__BottomToTop(int startY, int endY, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int totalsamples = son->m_img.height();

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(totalsamples);

    for (int y = startY; y < endY && !son->m_stop_sonifying; y++)
    {
        for (int x = 0; x < son->m_img.width(); x++)
            pixcols[x] = Pixel{ son->m_img.pixel(x, y), x, y };

        temp = son->m_mapFunc(pixcols);

        const int index = (totalsamples - 1 - y) * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__CircleOutwards(int startRadius, int endRadius,
                                            int &lastRadius, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int totalsamples = lastRadius;
    const int width        = son->m_img.width();
    const int height       = son->m_img.height();
    const double centerX   = width / 2.0;
    const double centerY   = height / 2.0;

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(360);

    for (int r = startRadius; r < endRadius && !son->m_stop_sonifying; r++)
    {
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x     = static_cast<int>(centerX + r * qCos(rad));
            int y     = static_cast<int>(centerY + r * qSin(rad));

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                pixcols[angle] = Pixel{ son->m_img.pixel(x, y), x, y };
            }
        }

        temp            = son->m_mapFunc(pixcols);
        const int index = r * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__CircleInwards(int startRadius, int endRadius,
                                           int &lastRadius, void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int width        = son->m_img.width();
    const int height       = son->m_img.height();
    const double centerX   = width / 2.0;
    const double centerY   = height / 2.0;
    const int totalsamples = lastRadius;

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(360);

    for (int r = startRadius; r < endRadius && !son->m_stop_sonifying; r++)
    {
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x     = static_cast<int>(centerX + r * qCos(rad));
            int y     = static_cast<int>(centerY + r * qSin(rad));

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                pixcols[angle] = Pixel{ son->m_img.pixel(x, y), x, y };
            }
        }

        temp            = son->m_mapFunc(pixcols);
        const int index = (lastRadius - r) * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__Clockwise(int startAngle, int endAngle,
                                       void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int width        = son->m_img.width();
    const int height       = son->m_img.height();
    const double centerX   = width / 2.0;
    const double centerY   = height / 2.0;
    const int totalsamples = 360;
    const int length =
        static_cast<int>(std::sqrt(centerX * centerX + centerY * centerY));

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(360);

    for (int angle = startAngle; angle < endAngle && !son->m_stop_sonifying;
         angle++)
    {
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x     = centerX + r * qCos(rad);
            int y     = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                pixcols[r] = Pixel{ son->m_img.pixel(x, y), x, y };
            }
        }

        temp            = son->m_mapFunc(pixcols);
        const int index = startAngle + angle * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::processImageChunk__AntiClockwise(int startAngle, int endAngle,
                                           void *s) noexcept
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    if (!son || son->m_stop_sonifying) return;

    const int nsamples     = son->m_nsamples;
    const int width        = son->m_img.width();
    const int height       = son->m_img.height();
    const double centerX   = width / 2.0;
    const double centerY   = height / 2.0;
    const int totalsamples = 360;
    const int length =
        static_cast<int>(std::sqrt(centerX * centerX + centerY * centerY));

    std::vector<short> temp;
    temp.resize(nsamples);

    std::vector<Pixel> pixcols;
    pixcols.resize(360);

    for (int angle = startAngle; angle < endAngle && !son->m_stop_sonifying;
         angle++)
    {
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x     = static_cast<int>(centerX + r * qCos(rad));
            int y     = static_cast<int>(centerY + r * qSin(rad));

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                pixcols[r] = Pixel{ son->m_img.pixel(x, y), x, y };
            }
        }

        temp            = son->m_mapFunc(pixcols);
        const int index = (360 - angle) * nsamples;

        std::copy_n(temp.cbegin(), nsamples, son->m_audioData.begin() + index);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(
            static_cast<int>(son->m_progressCounter.load() /
                             static_cast<double>(totalsamples) * 100));
    }
}

void
Sonifier::LeftToRight() noexcept
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.width());
    m_progressCounter = 0;

    int totalsamples = m_img.width();
    int numThreads   = QThreadPool::globalInstance()->maxThreadCount();
    std::vector<std::vector<short>> threadBuffers(numThreads);
    int chunkWidth = totalsamples / numThreads;

    auto completed = std::make_shared<std::atomic<int>>(0);

    for (int i = 0; i < numThreads; i++)
    {
        int startX = i * chunkWidth;
        int endX   = (i == numThreads - 1) ? totalsamples : startX + chunkWidth;

        (void)QtConcurrent::run([this, startX, endX, completed, numThreads]()
        {
            processImageChunk__LeftToRight(startX, endX, this);

            if (completed->fetch_add(1) + 1 == numThreads)
            {
                if (!m_stop_sonifying)
                {
                    QMetaObject::invokeMethod(this, [this]()
                    { emit sonificationDone(m_audioData); },
                                              Qt::QueuedConnection);
                }
            }
        });
    }
}

void
Sonifier::RightToLeft() noexcept
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.width());

    auto totalsamples = m_img.width();
    int numThreads    = QThreadPool::globalInstance()->maxThreadCount();
    int chunkWidth    = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;
    std::vector<short> temp;

    for (int i = 0; i < numThreads; i++)
    {
        int startX = i * chunkWidth;
        int endX   = (i == numThreads - 1) ? totalsamples : startX + chunkWidth;

        auto future = QtConcurrent::run(processImageChunk__RightToLeft, startX,
                                        endX, this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::TopToBottom() noexcept
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.height());

    auto totalsamples = m_img.height();
    int numThreads    = QThreadPool::globalInstance()->maxThreadCount();
    int chunkHeight   = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startY = i * chunkHeight;
        int endY = (i == numThreads - 1) ? totalsamples : startY + chunkHeight;

        auto future = QtConcurrent::run(processImageChunk__TopToBottom, startY,
                                        endY, this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::BottomToTop() noexcept
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.height());

    auto totalsamples = m_img.height();
    int numThreads    = QThreadPool::globalInstance()->maxThreadCount();
    int chunkHeight   = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startY = i * chunkHeight;
        int endY = (i == numThreads - 1) ? totalsamples : startY + chunkHeight;

        auto future = QtConcurrent::run(processImageChunk__BottomToTop, startY,
                                        endY, this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::CircleOutwards() noexcept
{
    int width  = m_img.width();
    int height = m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));
    auto totalsamples = radius;

    m_audioData.clear();
    m_audioData.resize(radius * m_nsamples);

    int numThreads  = QThreadPool::globalInstance()->maxThreadCount();
    int chunkRadius = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startRadius = i * chunkRadius;
        int endRadius =
            (i == numThreads - 1) ? totalsamples : startRadius + chunkRadius;

        auto future =
            QtConcurrent::run(processImageChunk__CircleOutwards, startRadius,
                              endRadius, std::ref(radius), this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::CircleInwards() noexcept
{
    int width  = m_img.width();
    int height = m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));
    auto totalsamples = radius;

    m_audioData.clear();
    m_audioData.resize(radius * m_nsamples);

    int numThreads  = QThreadPool::globalInstance()->maxThreadCount();
    int chunkRadius = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startRadius = i * chunkRadius;
        int endRadius =
            (i == numThreads - 1) ? totalsamples : startRadius + chunkRadius;

        auto future =
            QtConcurrent::run(processImageChunk__CircleInwards, startRadius,
                              endRadius, std::ref(radius), this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::Clockwise() noexcept
{
    std::vector<short> temp;
    std::vector<short> audioData;

    m_audioData.clear();
    m_audioData.resize(m_nsamples * 360);

    auto totalsamples = 360;
    int numThreads    = QThreadPool::globalInstance()->maxThreadCount();
    /*int numThreads = 1;*/
    int chunkAngle = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startAngle = i * chunkAngle;
        int endAngle =
            (i == numThreads - 1) ? totalsamples : startAngle + chunkAngle;

        auto future = QtConcurrent::run(processImageChunk__Clockwise,
                                        startAngle, endAngle, this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::AntiClockwise() noexcept
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * 360);

    auto totalsamples = 360;
    int numThreads    = QThreadPool::globalInstance()->maxThreadCount();
    int chunkAngle    = totalsamples / numThreads;

    std::vector<QFuture<void>> futures;

    for (int i = 0; i < numThreads; i++)
    {
        int startAngle = i * chunkAngle;
        int endAngle =
            (i == numThreads - 1) ? totalsamples : startAngle + chunkAngle;

        auto future = QtConcurrent::run(processImageChunk__AntiClockwise,
                                        startAngle, endAngle, this);
        futures.push_back(future);
    }

    for (auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying) emit sonificationDone(m_audioData);
}

void
Sonifier::PathDrawn() noexcept
{
    // m_audioData.clear();
    // std::vector<Pixel> pixcols;
    // pixcols.resize(m_pixpos.size());
    // std::vector<short> temp;
    // temp.resize(m_nsamples);
    //
    // for (int i = 0; i < m_pixpos.size(); i++)
    // {
    //     temp.clear();
    //     temp.resize(m_nsamples);
    //     if (m_stop_sonifying)
    //         return;
    //     auto pixelpos = m_pixpos[i];
    //     auto x        = pixelpos.x();
    //     auto y        = pixelpos.y();
    //
    //     /*pixcols[0] = Pixel { m_img.pixel(x, y), static_cast<int>(x),
    //      * static_cast<int>(y) };*/
    //     temp = addVectors(temp, m_mapping->Map__HSV(m_img.pixel(x, y) *
    //     200));
    //
    //     for (int j = 0; j < temp.size(); j++)
    //         m_audioData.push_back(temp[j]);
    //
    //     emit sonificationProgress(
    //         static_cast<int>(i / static_cast<double>(m_pixpos.size()) *
    //         100));
    // }
    //
    // if (!m_stop_sonifying)
    //     emit sonificationDone(m_audioData);
}

// If path drawn mode is selected, pass the pixels drawn.
void
Sonifier::setParameters(const QPixmap &pix, Traverse t,
                        const std::vector<QPointF> &pixels) noexcept
{
    m_traverse = t;
    m_pix      = pix;
    m_img      = m_pix.toImage();
    m_pixpos   = pixels;
}

// For other methods, no pixel pos is required
void
Sonifier::setParameters(const QPixmap &pix, Traverse t) noexcept
{
    m_traverse = t;
    m_pix      = pix;
    m_img      = m_pix.toImage();
}

// Function that handles which traversal method to call
void
Sonifier::Sonify() noexcept
{
    switch (m_traverse)
    {
        case Traverse::LEFT_TO_RIGHT: LeftToRight(); break;

        case Traverse::RIGHT_TO_LEFT: RightToLeft(); break;

        case Traverse::TOP_TO_BOTTOM: TopToBottom(); break;

        case Traverse::BOTTOM_TO_TOP: BottomToTop(); break;

        case Traverse::CLOCKWISE: Clockwise(); break;

        case Traverse::ANTICLOCKWISE: AntiClockwise(); break;

        case Traverse::CIRCLE_INWARDS: CircleInwards(); break;

        case Traverse::CIRCLE_OUTWARDS: CircleOutwards(); break;

        case Traverse::PATH: PathDrawn(); break;

        case Traverse::REGION: break;

        case Traverse::INSPECT: break;
    }
}

// Stop the sonification process
void
Sonifier::stopSonifying(bool state) noexcept
{
    m_stop_sonifying = state;
}
