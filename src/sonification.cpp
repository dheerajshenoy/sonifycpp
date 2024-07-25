#include "sonification.hpp"
#include <cmath>

Sonification::Sonification()
{

    m_val = M_PI2 * m_NumSamples / m_SampleRate;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

}

Sonification::~Sonification()
{
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
    }
    SDL_Quit();
}

void Sonification::setNumSamples(int nsamples)
{
    m_NumSamples = nsamples; 
    m_val = M_PI2 * m_NumSamples / m_SampleRate;
}

// Function to sonify an `image` provided by QImage and in mode `mode`
void Sonification::Sonify(QPixmap &pix, Traverse mode)
{

    // Return if null
    if (pix.isNull()) return;

    m_traverse = mode;

    SDL_zero(m_wavSpec);
    m_wavSpec.samples = m_NumSamples;
    m_wavSpec.freq = m_SampleRate;
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = m_ChannelCount;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;

    if (m_audioDevice)
        SDL_CloseAudioDevice(m_audioDevice);
    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);
    if (m_audioDevice == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
        return;
    }

    switch(mode)
    {
    
        case Traverse::LEFT_TO_RIGHT:
            m_Sonify_LeftToRight(pix);
            break;

        case Traverse::RIGHT_TO_LEFT:
            m_Sonify_RightToLeft(pix);
            break;

        case Traverse::TOP_TO_BOTTOM:
            m_Sonify_TopToBottom(pix);
            break;

        case Traverse::BOTTOM_TO_TOP:
            m_Sonify_BottomToTop(pix);
            break;

        case Traverse::CIRCLE_INWARDS:
            m_Sonify_CircleInwards(pix);
            break;

        case Traverse::CIRCLE_OUTWARDS:
            m_Sonify_CircleOutwards(pix);
            break;

        case Traverse::CLOCKWISE:
            m_Sonify_Clockwise(pix);
            break;

        case Traverse::ANTICLOCKWISE:
            m_Sonify_AntiClockwise(pix);
            break;
    }

}

QVector<double> Sonification::m_GenerateSineWave(double amplitude, double frequency, double time)
{
    QVector<double> fs;
    fs.resize(m_NumSamples);
    double val = m_val * frequency;
    for(int i=0; i < m_NumSamples; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}

// Left to right traversal
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_Sonify_LeftToRight(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();

    QImage img = pix.toImage();
    QVector<double> temp;
    for(int x=0; x < img.width(); x++)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            QColor col = QColor(pixel);
            double intensity = qGray(pixel);
            /*double intensity = (col.red() + col.green() + col.blue())/3.0;*/
            auto sine = m_GenerateSineWave(intensity, y, x);
            temp = addVectors<double>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }

}

// Right to Left traversal
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_Sonify_RightToLeft(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();

    QImage img = pix.toImage();
    QVector<double> temp;
    for(int x = img.width() - 1; x >= 0; x--)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            double intensity = qGray(pixel);
            auto sine = m_GenerateSineWave(intensity, y, x);
            temp = addVectors<double>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

// Top to Bottom
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_Sonify_TopToBottom(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();

    QImage img = pix.toImage();
    QVector<double> temp;
    for(int y = img.height() - 1; y >= 0; y--)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for(int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            double intensity = qGray(pixel);
            auto sine = m_GenerateSineWave(intensity, y, x);
            temp = addVectors<double>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

// Top to Bottom
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_Sonify_BottomToTop(QPixmap &pix)
{

    if (!m_audioData.isEmpty())
        m_audioData.clear();
    QImage img = pix.toImage();
    QVector<double> temp;
    for(int y = 0; y < img.height(); y++)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for(int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            double intensity = qGray(pixel);
            auto sine = m_GenerateSineWave(intensity, y, x);
            temp = addVectors<double>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

void Sonification::m_Sonify_CircleOutwards(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = pix.width() / 2.0;
    double centerY = pix.height() / 2.0;

    QImage img = pix.toImage();
    QVector<double> temp;

    auto radius = std::max(width / 2.0, height / 2.0);

    for (int r = 0; r < radius; r++)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                double intensity = qGray(pixel);
                auto sine = m_GenerateSineWave(intensity, y, x);
                temp = addVectors<double>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

void Sonification::m_Sonify_CircleInwards(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = pix.width() / 2.0;
    double centerY = pix.height() / 2.0;

    QImage img = pix.toImage();
    QVector<double> temp;

    auto radius = std::max(width / 2.0, height / 2.0);

    for (int r = radius - 1; r >= 0; r--)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for (int angle = 0; angle < 360; angle++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                double intensity = qGray(pixel);
                auto sine = m_GenerateSineWave(intensity, y, x);
                temp = addVectors<double>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

void Sonification::m_Sonify_Clockwise(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = pix.toImage();
    QVector<double> temp;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = 0; angle < 360; angle++)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                double intensity = qGray(pixel);
                auto sine = m_GenerateSineWave(intensity, y, x);
                temp = addVectors<double>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

void Sonification::m_Sonify_AntiClockwise(QPixmap &pix)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();
    int width = pix.width();
    int height = pix.height();
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    QImage img = pix.toImage();
    QVector<double> temp;

    auto length = static_cast<int>(sqrt(centerX * centerX + centerY * centerY));

    for (int angle = 359; angle >= 0; angle--)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for (int r = 0; r < length; r++)
        {
            qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
            int x = centerX + r * qCos(rad);
            int y = centerY + r * qSin(rad);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                QRgb pixel = img.pixel(x, y);
                double intensity = qGray(pixel);
                auto sine = m_GenerateSineWave(intensity, y, x);
                temp = addVectors<double>(temp, sine);
            }
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }
}

void Sonification::SonifyPath(QPixmap &pix, QVector<QPointF> pixelPos)
{
    if (!m_audioData.isEmpty())
        m_audioData.clear();

    /*QVector<QColor> pixels;*/
    /*pixels.resize(pixelPos.size());*/
    QImage img = pix.toImage();
    for(int i=0; i < pixelPos.size(); i++)
    {
        auto pixelpos = pixelPos[i];
        auto x = pixelpos.x();
        auto y = pixelpos.y();
        /*auto pixel = QColor(img.pixel(coord.x(), coord.y()));*/
        int intensity = qGray(img.pixel(x, y));
        auto sine = m_GenerateSineWave(intensity, y, x);

        for(int j=0; j < sine.size(); j++)
            m_audioData.push_back(sine[j]);
    }

}

template <typename T>
QVector<T> Sonification::addVectors(QVector<T> &a, QVector<T> &b)
{
    if (a.length() != b.length()) return {};
    QVector<T> res;
    res.resize(a.length());

    for(int i=0; i < a.length(); i++)
    {
        res[i] = a[i] + b[i];
    }
    return res;
}

void Sonification::pause()
{
    SDL_PauseAudioDevice(m_audioDevice, 1);
}

void Sonification::play()
{
    SDL_PauseAudioDevice(m_audioDevice, 0);
}

void Sonification::reset()
{
    SDL_LockAudioDevice(m_audioDevice); // Lock the audio device to avoid race conditions
    m_audioOffset = 0; // Reset the audio offset
    SDL_UnlockAudioDevice(m_audioDevice); // Unlock the audio device
    SDL_PauseAudioDevice(m_audioDevice, 1);
}


/*double Sonification::m_GenerateSineWave(double amplitude, double frequency, double time)*/
/*{*/
/*    return amplitude * std::sin(m_val * frequency * time);*/
/*}*/

double Sonification::m_MapIntensityToFrequence(int intensity)
{
    return 200.0 + (intensity * 800.0 / 255.0);
}

bool Sonification::save(QString filename, Format f)
{
    switch(f)
    {
        case Format::WAV:
            if(m_GenerateWavFile(filename))
                return true;
            break;

        case Format::MP3:
            break;
    }

    return false;
}

bool Sonification::m_GenerateWavFile(QString filename)
{
    SF_INFO sfinfo;
    sfinfo.samplerate = m_SampleRate;
    sfinfo.frames = getDuration();
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.channels = 1;

    SNDFILE *sndfile = sf_open(filename.toStdString().c_str(), SFM_WRITE, &sfinfo);

    if (!sndfile) {
        qDebug() << "Error writing to file: Error %d" << sf_error(sndfile);
        m_audioData.clear();
        return false;
    }

    sf_count_t count = sf_write_short(sndfile, m_audioData.data(), m_audioData.size());

    /*if (count != m_NumSamples)*/
    /*{*/
    /*    fprintf(stderr, "Error writing to file: Error %d", sf_error(sndfile));*/
    /*    sf_close(sndfile);*/
    /*    m_audioData.clear();*/
    /*    return false;*/
    /*}*/

    sf_close(sndfile);

    return true;
}

double Sonification::getDuration()
{
    return m_audioData.size() / m_SampleRate;
}

void Sonification::sdlAudioCallback(void* userdata, Uint8* stream, int len)
{
    Sonification *s = static_cast<Sonification*>(userdata);

    int bytesToCopy = std::min(static_cast<int>(s->m_audioData.size() * sizeof(short) - s->m_audioOffset), len);
    
    memcpy(stream, s->m_audioData.data() + s->m_audioOffset / sizeof(short), bytesToCopy);

    s->m_audioOffset += bytesToCopy;

    if (bytesToCopy < len) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
    }

    /*if (s->m_audioOffset >= s->m_audioData.size() * sizeof(short)) {*/
    /*    s->m_audioOffset = 0;*/
    /*    SDL_PauseAudioDevice(s->m_audioDevice, 1);*/
    /*}*/
}

int Sonification::getNumSamples()
{
    return m_NumSamples;
}

float Sonification::getSampleRate()
{
    return m_SampleRate;
}


QVector<short> Sonification::getAudioData()
{
    return m_audioData;
}
