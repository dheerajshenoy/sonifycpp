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

void Sonifier::LeftToRight(QPixmap &pix, QVector<short> &audioData)
{
    QImage img = pix.toImage();

    if (!audioData.isEmpty())
        audioData.clear();

    /*audioData.resize(img.width() * m_nsamples);*/
    QVector<short> temp;

    for(int x=0; x < img.width(); x++)
    {
        temp.clear();
        temp.resize(m_nsamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            auto sine = m_mapping->Map1(pixel, y, x);
            temp = utils::addVectors<short>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));
    }

    emit sonificationDone();
}

void Sonifier::RightToLeft(QPixmap &pix, QVector<short> &audioData)
{
    QImage img = pix.toImage();
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

        for(int i=0; i < temp.size(); i++)
            audioData.push_back(temp.at(i));
    }

    emit sonificationDone();

}
void Sonifier::TopToBottom(QPixmap &pix, QVector<short> &audioData)
{

    QImage img = pix.toImage();
    if (!audioData.isEmpty())
        audioData.clear();

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
    }

    emit sonificationDone();
}

// Top to Bottom
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonifier::BottomToTop(QPixmap &pix, QVector<short> &audioData)
{

    if (!audioData.isEmpty())
        audioData.clear();
    QImage img = pix.toImage();
    QVector<short> temp;
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
    }

    emit sonificationDone();
}


void Sonifier::CircleOutwards(QPixmap &pix, QVector<short> &audioData)
{
    if (!audioData.isEmpty())
        audioData.clear();

    int width = pix.width();
    int height = pix.height();

    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = pix.toImage();
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
    }

    emit sonificationDone();
}

void Sonifier::CircleInwards(QPixmap &pix, QVector<short> &audioData)
{
    if (!audioData.isEmpty())
        audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = pix.width() / 2.0;
    double centerY = pix.height() / 2.0;

    QImage img = pix.toImage();
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
    }

    emit sonificationDone();
}


void Sonifier::Clockwise(QPixmap &pix, QVector<short> &audioData)
{
    if (!audioData.isEmpty())
        audioData.clear();

    int width = pix.width();
    int height = pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = pix.toImage();
    QVector<short> temp;

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
    }
}

void Sonifier::AntiClockwise(QPixmap &pix, QVector<short> &audioData)
{
    if (!audioData.isEmpty())
        audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = pix.toImage();
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
    }

    emit sonificationDone();
}

void Sonifier::PathDrawn(QVector<QPointF> &pixelPos, QPixmap &pix, QVector<short> &audioData)
{

    if (!audioData.isEmpty())
        audioData.clear();

    QImage img = pix.toImage();
    for(int i=0; i < pixelPos.size(); i++)
    {
        auto pixelpos = pixelPos[i];
        auto x = pixelpos.x();
        auto y = pixelpos.y();
        int intensity = qGray(img.pixel(x, y));
        auto sine = m_mapping->Map1(intensity * 200, y, x);
        for(int j=0; j < sine.size(); j++)
            audioData.push_back(sine[j]);
    }

    emit sonificationDone();
}
