/*
* Contains the code for different traversal types.
*/

#include "sonifier.hpp"

void Sonifier::setSamples(int nsamples) noexcept
{
    m_nsamples = nsamples;
    m_mapping->setSamples(nsamples);
}

void Sonifier::setSampleRate(float SR) noexcept
{
    m_SampleRate = SR;
    m_mapping->setSampleRate(SR);
}

void Sonifier::setFreqMap(FreqMap &m) noexcept
{
    m_mapping->setFreqMap(m);
}

void Sonifier::setMinMax(int &min, int &max) noexcept
{
    m_mapping->setMinMax(min, max);
}

void Sonifier::processImageChunk__LeftToRight(int startX, int endX, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(son->m_img.height());
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);
    int totalsamples = son->m_img.width();
    for (int x = startX; x < endX; x++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);

        for(int y=0; y < son->m_img.height(); y++)
            pixcols[y] = PixelColumn { son->m_img.pixel(x, y), x, y };

        temp = son->m_mapping->MapFull2(pixcols);

        int index = startX + x * nsamples;

        for(int i=0; i < temp.size(); i++)
            son->m_audioData[index + i] = temp.at(i);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);

    }
}

void Sonifier::processImageChunk__RightToLeft(int startX, int endX, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(son->m_img.height());
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);
    int totalsamples = son->m_img.width();

    for (int x = startX; x < endX; x++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);
        for (int y = 0; y < son->m_img.height(); y++)
            pixcols[y] = PixelColumn { son->m_img.pixel(x, y), x, y };

        temp = son->m_mapping->MapFull2(pixcols);

        int index = (totalsamples - 1 - x ) * nsamples;

        for(int i=0; i < temp.size(); i++)
        {
            son->m_audioData[index + i] = temp.at(i);
        }

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);

    }
}

void Sonifier::processImageChunk__TopToBottom(int startY, int endY, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(son->m_img.height());
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);
    int totalsamples = son->m_img.height();
    for (int y = startY; y < endY; y++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);
        for (int x = 0; x < son->m_img.width(); x++)
            pixcols[x] = PixelColumn { son->m_img.pixel(x, y), x, y };

        temp = son->m_mapping->MapFull2(pixcols);

        int index = startY + y * nsamples;

        for(int i=0; i < temp.size(); i++)
            son->m_audioData[index + i] = temp.at(i);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);

    }
}

void Sonifier::processImageChunk__BottomToTop(int startY, int endY, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(son->m_img.height());
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);
    int totalsamples = son->m_img.height();
    for (int y = startY; y < endY; y++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);
        for (int x = 0; x < son->m_img.width(); x++)
            pixcols[x] = PixelColumn { son->m_img.pixel(x, y), x, y };

        temp = son->m_mapping->MapFull2(pixcols);

        int index = (totalsamples - 1 - y ) * nsamples;

        for(int i=0; i < temp.size(); i++)
            son->m_audioData[index + i] = temp.at(i);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);

    }
}

void Sonifier::processImageChunk__CircleOutwards(int startRadius, int endRadius, int &lastRadius, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(360);
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);

    int width = son->m_img.width();
    int height = son->m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    int totalsamples = lastRadius;

    for (int r = startRadius; r < endRadius; r++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = static_cast<int>(centerX + r * qCos(rad));
            int y = static_cast<int>(centerY + r * qSin(rad));

            if (x >= 0 && x < width && y >= 0 && y < height) {
                pixcols[angle] = PixelColumn { son->m_img.pixel(x, y), x, y };
            }
        }

        temp = son->m_mapping->MapFull2(pixcols);
        int index = r * nsamples;

        for(int i=0; i < temp.size(); i++)
        {
            son->m_audioData[index + i] = temp.at(i);
        }


        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);
    }
}

void Sonifier::processImageChunk__CircleInwards(int startRadius, int endRadius, int &lastRadius, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(360);
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);

    int width = son->m_img.width();
    int height = son->m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    int totalsamples = lastRadius;

    for (int r = startRadius; r < endRadius; r++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(nsamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = static_cast<int>(centerX + r * qCos(rad));
            int y = static_cast<int>(centerY + r * qSin(rad));

            if (x >= 0 && x < width && y >= 0 && y < height) {
                pixcols[angle] = PixelColumn { son->m_img.pixel(x, y), x, y };
            }
        }

        temp = son->m_mapping->MapFull2(pixcols);
        int index = (lastRadius - r) * nsamples;

        for(int i=0; i < temp.size(); i++)
        {
            son->m_audioData[index + i] = temp.at(i);
        }


        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);
    }
}

void Sonifier::processImageChunk__Clockwise(int startAngle, int endAngle, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(360);
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);

    int width = son->m_img.width();
    int height = son->m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    int totalsamples = 360;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = startAngle; angle < endAngle; angle++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(son->m_nsamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                pixcols[r] = PixelColumn { son->m_img.pixel(x, y), x, y };
            }
        }

        temp = son->m_mapping->MapFull2(pixcols);
        int index = startAngle + angle * nsamples;

        for(int i=0; i < temp.size(); i++)
            son->m_audioData[index + i] = temp.at(i);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);
    }
}

void Sonifier::processImageChunk__AntiClockwise(int startAngle, int endAngle, void *s)
{
    Sonifier *son = reinterpret_cast<Sonifier *>(s);
    QVector<short> temp;
    QVector<PixelColumn> pixcols;
    pixcols.resize(360);
    auto nsamples = son->m_nsamples;
    temp.resize(nsamples);

    int width = son->m_img.width();
    int height = son->m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    int totalsamples = 360;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = startAngle; angle < endAngle; angle++)
    {
        if (son->m_stop_sonifying) return;
        temp.clear();
        temp.resize(son->m_nsamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                pixcols[r] = PixelColumn { son->m_img.pixel(x, y), x, y };
            }
        }

        temp = son->m_mapping->MapFull2(pixcols);
        int index = (360 - angle ) * nsamples;

        for(int i=0; i < temp.size(); i++)
            son->m_audioData[index + i] = temp.at(i);

        QMutexLocker locker(&son->m_mutex);
        son->m_progressCounter++;

        emit son->sonificationProgress(son->m_progressCounter.load() / static_cast<double>(totalsamples) * 100);
    }
}

void Sonifier::LeftToRight()
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.width());

    auto totalsamples = m_img.width();
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkWidth = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startX = i * chunkWidth;
        int endX = (i == numThreads - 1) ? totalsamples : startX + chunkWidth;

        auto future = QtConcurrent::run(processImageChunk__LeftToRight, startX, endX, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::RightToLeft()
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.width());

    auto totalsamples = m_img.width();
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkWidth = totalsamples / numThreads;

    QVector<QFuture<void>> futures;
    QVector<short> temp;

    for(int i=0; i < numThreads; i++)
    {
        int startX = i * chunkWidth;
        int endX = (i == numThreads - 1) ? totalsamples : startX + chunkWidth;

        auto future = QtConcurrent::run(processImageChunk__RightToLeft, startX, endX, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);

}

void Sonifier::TopToBottom()
{

    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.height());

    auto totalsamples = m_img.height();
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkHeight = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startY = i * chunkHeight;
        int endY = (i == numThreads - 1) ? totalsamples : startY + chunkHeight;

        auto future = QtConcurrent::run(processImageChunk__TopToBottom, startY, endY, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::BottomToTop()
{

    m_audioData.clear();
    m_audioData.resize(m_nsamples * m_img.height());

    auto totalsamples = m_img.height();
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkHeight = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startY = i * chunkHeight;
        int endY = (i == numThreads - 1) ? totalsamples : startY + chunkHeight;

        auto future = QtConcurrent::run(processImageChunk__BottomToTop, startY, endY, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::CircleOutwards()
{

    int width = m_img.width();
    int height = m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));
    auto totalsamples = radius;

    m_audioData.clear();
    m_audioData.resize(radius * m_nsamples);

    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkRadius = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startRadius = i * chunkRadius;
        int endRadius = (i == numThreads - 1) ? totalsamples : startRadius + chunkRadius;

        auto future = QtConcurrent::run(processImageChunk__CircleOutwards, startRadius,
                                        endRadius, std::ref(radius), this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::CircleInwards()
{

    int width = m_img.width();
    int height = m_img.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));
    auto totalsamples = radius;

    m_audioData.clear();
    m_audioData.resize(radius * m_nsamples);

    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkRadius = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startRadius = i * chunkRadius;
        int endRadius = (i == numThreads - 1) ? totalsamples : startRadius + chunkRadius;

        auto future = QtConcurrent::run(processImageChunk__CircleInwards, startRadius,
                                        endRadius, std::ref(radius), this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::Clockwise()
{
    QVector<short> temp;
    QVector<short> audioData;

    m_audioData.clear();
    m_audioData.resize(m_nsamples * 360);

    auto totalsamples = 360;
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    /*int numThreads = 1;*/
    int chunkAngle = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startAngle = i * chunkAngle;
        int endAngle = (i == numThreads - 1) ? totalsamples : startAngle + chunkAngle;

        auto future = QtConcurrent::run(processImageChunk__Clockwise, startAngle, endAngle, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);

}

void Sonifier::AntiClockwise()
{
    m_audioData.clear();
    m_audioData.resize(m_nsamples * 360);

    auto totalsamples = 360;
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();
    int chunkAngle = totalsamples / numThreads;

    QVector<QFuture<void>> futures;

    for(int i=0; i < numThreads; i++)
    {
        int startAngle = i * chunkAngle;
        int endAngle = (i == numThreads - 1) ? totalsamples : startAngle + chunkAngle;

        auto future = QtConcurrent::run(processImageChunk__AntiClockwise, startAngle, endAngle, this);
        futures.push_back(future);

    }

    for(auto &future : futures)
        future.waitForFinished();

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

void Sonifier::PathDrawn()
{
    m_audioData.clear();
    /*m_audioData.resize(m_pixpos.size() * m_nsamples);*/
    QVector<PixelColumn> pixcols;
    pixcols.resize(m_pixpos.size());
    QVector<short> temp;
    temp.resize(m_nsamples);

    for(int i=0; i < m_pixpos.size(); i++)
    {
        temp.clear();
        temp.resize(m_nsamples);
        if (m_stop_sonifying) return;
        auto pixelpos = m_pixpos[i];
        auto x = pixelpos.x();
        auto y = pixelpos.y();

        /*pixcols[0] = PixelColumn { m_img.pixel(x, y), static_cast<int>(x), static_cast<int>(y) };*/
        temp = utils::addVectors(temp, m_mapping->Map1(m_img.pixel(x, y) * 200, x, y));

        for(int j=0; j < temp.size(); j++)
            m_audioData.push_back(temp[j]);

        emit sonificationProgress(i / static_cast<double>(m_pixpos.size()) * 100);

    }

    if (!m_stop_sonifying)
        emit sonificationDone(m_audioData);
}

// If path drawn mode is selected, pass the pixels drawn.
void Sonifier::setParameters(QPixmap &pix, Traverse t, QVector<QPointF> pixels)
{
    m_traverse = t;
    m_pix = pix;
    m_img = m_pix.toImage();
    m_pixpos = pixels;
}

// For other methods, no pixel pos is required
void Sonifier::setParameters(QPixmap &pix, Traverse t)
{
    m_traverse = t;
    m_pix = pix;
    m_img = m_pix.toImage();
}

// Function that handles which traversal method to call
void Sonifier::Sonify()
{

    switch(m_traverse)
    {
        case Traverse::LEFT_TO_RIGHT:
            LeftToRight();
            break;

        case Traverse::RIGHT_TO_LEFT:
            RightToLeft();
            break;

        case Traverse::TOP_TO_BOTTOM:
            TopToBottom();
            break;

        case Traverse::BOTTOM_TO_TOP:
            BottomToTop();
            break;

        case Traverse::CLOCKWISE:
            Clockwise();
            break;

        case Traverse::ANTICLOCKWISE:
            AntiClockwise();
            break;

        case Traverse::CIRCLE_INWARDS:
            CircleInwards();
            break;

        case Traverse::CIRCLE_OUTWARDS:
            CircleOutwards();
            break;

        case Traverse::PATH:
            PathDrawn();
            break;

        case Traverse::REGION:
            break;
    }
}

// Stop the sonification process
void Sonifier::stopSonifying(bool state)
{
    m_stop_sonifying = state;
    m_audioData.squeeze();
}
