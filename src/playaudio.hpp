#ifndef __PLAYAUDIO_HPP
#define __PLAYAUDIO_HPP

class PlayAudio : public QObject
{
Q_OBJECT

    public:
        PlayAudio(std::vector<float> song, int sample_rate, int samples, float duration);
        virtual ~PlayAudio();

    public slots:
        void Play();

    private:
        float m_duration;
        int m_samples, m_samplerate;

    signals:
        void progress(int);
        void finished(bool);

};

#endif
