#ifndef __PLAYAUDIO_CPP
#define __PLAYAUDIO_CPP

#include "playaudio.hpp"

std::vector<float> audioData;

// Audio callback function
void audioCallback(void* userdata, Uint8* stream, int len) {
    // std::vector<float>* audioData = static_cast<std::vector<float>*>(userdata);
    float* streamData = reinterpret_cast<float*>(stream);
    
    int numFrames = len / sizeof(float);
    int i = 0;
    
    while (i < numFrames) {
        streamData[i] = audioData.at(i);
        // audioData.erase(audioData.begin());
        i++;
    }
}

PlayAudio::PlayAudio(std::vector<float> song, int sample_rate, int samples, float duration)
    : m_duration(duration),
    m_samples(samples),
    m_samplerate(sample_rate)
{
    audioData = song;
}

void PlayAudio::Play()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize SDL audio
    SDL_AudioSpec audioSpec;
    audioSpec.freq = m_samplerate;
    audioSpec.format = AUDIO_F32;
    audioSpec.channels = 1; // Mono audio
    audioSpec.samples = m_samples; // Adjust as needed
    audioSpec.callback = audioCallback;
    // audioSpec.userdata = this;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, 0, &audioSpec, nullptr, 0);
    if (audioDevice == 0) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudioDevice(audioDevice, 0); // Start audio playback
                                          
    SDL_Delay(m_duration);

    SDL_PauseAudioDevice(audioDevice, 1); // Pause audio playback
    SDL_CloseAudioDevice(audioDevice);

    SDL_Quit();
}

PlayAudio::~PlayAudio()
{

}

#endif
