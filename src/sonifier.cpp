#include "sonifier.hpp"


void Sonifier::setSamples(int nsamples)
{
    m_nsamples = nsamples;
    m_mapping->setSamples(nsamples);
}

void Sonifier::setSampleRate(float SR)
{
    m_SampleRate = SR;
    m_mapping->setSampleRate(SR);
}

void Sonifier::LeftToRight()
{
    if (m_stop_sonifying) return;
    QImage img = m_pix.toImage();

    QVector<short> temp;
    QVector<short> audioData;
    /*audioData.resize(img.width() * m_nsamples);*/
    auto totalsamples = img.width();

    for(int x=0; x < img.width(); x++)
    {
        if (m_stop_sonifying) return;
        temp.clear();
        temp.resize(m_nsamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            auto sine = m_mapping->Map2(pixel, y, x);
            temp = utils::addVectors<short>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(x / static_cast<double>(totalsamples) * 100);
    }

    emit sonificationDone(audioData);
}

void Sonifier::RightToLeft()
{
    QImage img = m_pix.toImage();

    QVector<short> audioData;
    /*audioData.resize(img.width() * m_nsamples);*/
    QVector<short> temp;

    for(int x = img.width() - 1; x >= 0; x--)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            auto sine = m_mapping->Map1(pixel, y, x);
            temp = utils::addVectors<short>(temp, sine);
        }

        emit sonificationProgress(x);
        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));
    }

    emit sonificationDone(audioData);

}
void Sonifier::TopToBottom()
{

    QImage img = m_pix.toImage();

    QVector<short> audioData;

    QVector<short> temp;
    for(int y = 0; y < img.height(); y++)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for(int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            auto sine = m_mapping->Map1(pixel, y, x);
            temp = utils::addVectors<short>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(y);
    }

    emit sonificationDone(audioData);
}

// Top to Bottom
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonifier::BottomToTop()
{

    QImage img = m_pix.toImage();
    QVector<short> temp;
    QVector<short> audioData;
    for(int y = img.height() - 1; y >= 0; y--)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for(int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            auto sine = m_mapping->Map1(pixel, y, x);
            temp = utils::addVectors<short>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(y);
    }

    emit sonificationDone(audioData);
}


void Sonifier::CircleOutwards()
{
    int width = m_pix.width();
    int height = m_pix.height();

    QVector<short> audioData;

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = m_pix.toImage();
    QVector<short> temp;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int r = 0; r < radius; r++)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                auto sine = m_mapping->Map1(pixel, y, x);
                temp = utils::addVectors<short>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(r);
    }

    emit sonificationDone(audioData);
}

void Sonifier::CircleInwards()
{
    QVector<short> audioData;
    int width = m_pix.width();
    int height = m_pix.height();
    double centerX = m_pix.width() / 2.0;
    double centerY = m_pix.height() / 2.0;

    QImage img = m_pix.toImage();
    QVector<short> temp;

    auto radius = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int r = radius - 1; r >= 0; r--)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                auto sine = m_mapping->Map1(pixel, y, x);
                temp = utils::addVectors<short>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(r);
    }

    emit sonificationDone(audioData);
}


void Sonifier::Clockwise()
{
    int width = m_pix.width();
    int height = m_pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = m_pix.toImage();
    QVector<short> temp;
    QVector<short> audioData;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = 0; angle < 360; angle++)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                auto sine = m_mapping->Map1(pixel, y, x);
                temp = utils::addVectors<short>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));

        emit sonificationProgress(angle);
    }
    emit sonificationDone(audioData);
}

void Sonifier::AntiClockwise()
{
    int width = m_pix.width();
    int height = m_pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QVector<short> audioData;
    QImage img = m_pix.toImage();
    QVector<short> temp;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = 359; angle >= 0; angle--)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                auto sine = m_mapping->Map1(pixel, y, x);
                temp = utils::addVectors<short>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(static_cast<short>(temp.at(i)));

        emit sonificationProgress(angle);
    }

    emit sonificationDone(audioData);
}

void Sonifier::PathDrawn()
{
    QVector<short> audioData;
    QImage img = m_pix.toImage();
    for(int i=0; i < m_pixpos.size(); i++)
    {
        auto pixelpos = m_pixpos[i];
        auto x = pixelpos.x();
        auto y = pixelpos.y();
        int intensity = qGray(img.pixel(x, y));
        auto sine = m_mapping->Map1(intensity * 200, y, x);
        for(int j=0; j < sine.size(); j++)
            audioData.push_back(sine[j]);
        emit sonificationProgress(i);
    }

    emit sonificationDone(audioData);
}

void Sonifier::setParameters(QPixmap &pix, Traverse t, QVector<QPointF> &pixels)
{
    m_traverse = t;
    m_pix = pix;
    m_pixpos = pixels;
}


void Sonifier::setParameters(QPixmap &pix, Traverse t)
{
    m_traverse = t;
    m_pix = pix;
}

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

void Sonifier::stopSonifying(bool state)
{
    m_stop_sonifying = state;
}
