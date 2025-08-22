#include "utils_internal.hpp"

#include <complex>
#include <sndfile.h>

namespace utils_internal
{
    QImage changeGamma(const QImage &input, int gammaValue) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);
        double gamma = gammaValue / 100.0;

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r =
                    qBound(0, int(std::pow(c.red() / 255.0, gamma) * 255), 255);
                int g = qBound(0, int(std::pow(c.green() / 255.0, gamma) * 255),
                               255);
                int b = qBound(0, int(std::pow(c.blue() / 255.0, gamma) * 255),
                               255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

    QImage changeBrightness(const QImage &input, int delta) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r   = qBound(0, c.red() + delta, 255);
                int g   = qBound(0, c.green() + delta, 255);
                int b   = qBound(0, c.blue() + delta, 255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

    QImage changeSaturation(const QImage &input, int delta) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int h, s, v;
                c.getHsv(&h, &s, &v);
                s = qBound(0, s + delta, 255);
                c.setHsv(h, s, v);
                line[x] = c.rgb();
            }
        }

        return image;
    }

    QImage invertColor(const QImage &input) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                line[x] = qRgb(255 - c.red(), 255 - c.green(), 255 - c.blue());
            }
        }

        return image;
    }

    QImage convertToGrayscale(const QImage &input) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int gray = qGray(c.rgb());
                line[x]  = qRgb(gray, gray, gray);
            }
        }

        return image;
    }

    QImage changeContrast(const QImage &input, int contrastValue) noexcept
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        double factor = (259.0 * (contrastValue + 255.0)) /
                        (255.0 * (259.0 - contrastValue));

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r   = qBound(0, int(factor * (c.red() - 128) + 128), 255);
                int g   = qBound(0, int(factor * (c.green() - 128) + 128), 255);
                int b   = qBound(0, int(factor * (c.blue() - 128) + 128), 255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

    // Read in a WAV file. This was used just to check if the
    // Waveforms visualizer worked
    std::vector<short> readWAVFile(const QString &filename) noexcept
    {
        SF_INFO sfinfo;

        SNDFILE *file =
            sf_open(filename.toStdString().c_str(), SFM_READ, &sfinfo);

        if (!file)
        {
            fprintf(stderr, "Could not open the file");
            return {};
        }

        std::vector<short> data;
        data.resize(sfinfo.frames);

        sf_read_short(file, data.data(), sfinfo.frames);
        sf_close(file);

        return data;
    }

    // Helper function to generate WAV file
    bool generateWavFile(const QString &filename, int sampleRate, int duration,
                         const std::vector<short> &data) noexcept
    {
        SF_INFO sfinfo;
        sfinfo.samplerate = sampleRate;
        sfinfo.frames     = duration;
        sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        sfinfo.channels   = 1;

        SNDFILE *sndfile =
            sf_open(filename.toStdString().c_str(), SFM_WRITE, &sfinfo);

        if (!sndfile) { return false; }

        // sf_count_t count
        //     = sf_write_short(sndfile, m_audioData.data(),
        //     m_audioData.size());

        /*if (count != m_NumSamples)*/
        /*{*/
        /*    fprintf(stderr, "Error writing to file: Error %d",
         * sf_error(sndfile));*/
        /*    sf_close(sndfile);*/
        /*    m_audioData.clear();*/
        /*    return false;*/
        /*}*/

        sf_close(sndfile);

        return true;
    }

} // namespace utils_internal
