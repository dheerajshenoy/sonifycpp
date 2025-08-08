#include "utils.hpp"

#include <complex>
#include <sndfile.h>

namespace utils
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

    short LinearMap(double inp_min, double inp_max, double out_min,
                    double out_max, double val) noexcept
    {
        return static_cast<short>(out_min + (val - inp_min) *
                                                (out_max - out_min) /
                                                (inp_max - inp_min));
    }

    short ExpMap(double inp_min, double inp_max, double out_min, double out_max,
                 double val) noexcept
    {
        double x_norm = (val - inp_min) / (inp_max - inp_min);
        double y_norm = std::pow(x_norm, 2);
        return static_cast<short>(y_norm * (out_max - out_min) + out_min);
    }

    short LogMap(double inp_min, double inp_max, double out_min, double out_max,
                 double val) noexcept
    {
        double x_norm = (val - inp_min) / (inp_max - inp_min);
        double y_norm = std::log(x_norm + 1) / std::log(10.0);
        return static_cast<short>(y_norm * (out_max - out_min) + out_min);
    }

    void generateSineWave(std::vector<short> &vector, double _amplitude,
                          double frequency, double time,
                          float samplerate) noexcept
    {
        const unsigned int N = static_cast<unsigned int>(samplerate * time);
        if (N == 0) return;

        vector.resize(N);

        // clamp amplitude to avoid overflow
        double amp         = std::clamp(_amplitude, -1.0, 1.0);
        const double scale = amp * 32767.0;

        const double theta =
            2.0 * M_PI * frequency / static_cast<double>(samplerate);

        // rotation complex number: e^{i θ}
        const std::complex<double> w(std::cos(theta), std::sin(theta));
        std::complex<double> z(0.0, 0.0);
        // start at angle zero: sin(0)=0, but we want first sample as sin(0),
        // so initialize z = (cos(0), sin(0)) = (1,0), then advance after using.
        z = std::complex<double>(1.0, 0.0);

        for (unsigned int i = 0; i < N; ++i)
        {
            vector[i] = static_cast<short>(scale * z.imag());
            z *= w; // advance phase
        }
    }

    std::vector<short> sineWave(double _amplitude, double frequency,
                                double time, float samplerate) noexcept
    {
        const unsigned int N = static_cast<unsigned int>(samplerate * time);
        if (N == 0) return {};

        std::vector<short> fs;
        fs.resize(N);

        // clamp amplitude to avoid overflow
        double amp         = std::clamp(_amplitude, -1.0, 1.0);
        const double scale = amp * 32767.0;

        const double theta =
            2.0 * M_PI * frequency / static_cast<double>(samplerate);
        // rotation complex number: e^{i θ}
        const std::complex<double> w(std::cos(theta), std::sin(theta));
        std::complex<double> z(0.0, 0.0);
        // start at angle zero: sin(0)=0, but we want first sample as sin(0),
        // so initialize z = (cos(0), sin(0)) = (1,0), then advance after using.
        z = std::complex<double>(1.0, 0.0);

        for (unsigned int i = 0; i < N; ++i)
        {
            double sample = z.imag(); // sin(current angle)
            fs[i]         = static_cast<short>(scale * sample);
            z *= w; // advance phase
        }

        return fs;
    }

    void applyEnvelope(std::vector<short> &samples) noexcept
    {

        const int N       = static_cast<int>(samples.size());
        const int attack  = std::min(100, N / 10);
        const int release = std::min(100, N / 10);

        for (int i = 0; i < attack; ++i)
            samples[i] *= static_cast<float>(i) / attack;

        for (int i = 0; i < release; ++i)
            samples[N - 1 - i] *= static_cast<float>(i) / release;
    }

    double Hue2Freq(int H) noexcept
    {
        constexpr double scale_freqs[] = { 220.00, 246.94, 261.63, 293.66,
                                           329.63, 349.23, 415.30 };
        constexpr int thresholds[]     = { 26, 52, 78, 104, 128, 154, 180 };
        auto note                      = scale_freqs[0];

        if (H <= thresholds[0])
            note = scale_freqs[0];

        else if (H > thresholds[0] && H <= thresholds[1])
            note = scale_freqs[1];

        else if (H > thresholds[1] && H <= thresholds[2])
            note = scale_freqs[2];

        else if (H > thresholds[2] && H <= thresholds[3])
            note = scale_freqs[3];

        else if (H > thresholds[3] && H <= thresholds[4])
            note = scale_freqs[4];

        else if (H > thresholds[4] && H <= thresholds[5])
            note = scale_freqs[5];

        else if (H > thresholds[5] && H <= thresholds[6])
            note = scale_freqs[6];

        else
            note = scale_freqs[0];

        return note;
    }

    // Function that is similar to numpy linspace.
    std::vector<double> linspace(double start, double stop, int num) noexcept
    {
        std::vector<double> result;
        result.reserve(num);
        double step = (stop - start) / (num - 1);

        for (int i = 0; i < num; ++i)
        {
            result[i] = start + i * step;
        }

        return result;
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

    void applyFadeInOut(std::vector<short> &wave, int fade) noexcept
    {
        const size_t N  = wave.size();
        size_t fade_len = N / static_cast<size_t>(fade); // 10% fade

        for (size_t i = 0; i < fade_len; ++i)
        {
            double factor = static_cast<double>(i) / fade_len;
            wave[i] *= static_cast<short>(factor);
            wave[N - 1 - i] *= static_cast<short>(factor);
        }
    }

    void normalizeWave(std::vector<short> &wave) noexcept
    {
        short max_val = 1;
        for (short v : wave)
            max_val = std::max(max_val, static_cast<short>(std::abs(v)));
        if (max_val == 0 || max_val >= std::numeric_limits<short>::max())
            return;

        double scale =
            std::numeric_limits<short>::max() / static_cast<double>(max_val);
        for (short &v : wave)
            v = static_cast<short>(v * scale);
    }

    int intensity(QRgb pixel) noexcept
    {
        return qGray(pixel);
    }

} // namespace utils
