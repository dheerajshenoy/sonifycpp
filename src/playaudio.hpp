#ifndef __PLAYAUDIO_HPP
#define __PLAYAUDIO_HPP

class PlayAudio : public QThread
{
    public:
        PlayAudio();
        ~PlayAudio();
        void run();

    private:
};


#endif
