#include "audio/audio_capture.h"
#include <portaudio.h>
#include <iostream>

namespace jarvis {

class AudioCapture::AudioCaptureImpl {
public:
    PaStream* stream_ = nullptr;
    AudioCapture::AudioCallback callback_;
    bool running_ = false;
    int sampleRate_ = 16000;
    int channels_ = 1;
    int framesPerBuffer_ = 1024;
};

AudioCapture::AudioCapture() : impl_(std::make_unique<AudioCaptureImpl>()) {}
AudioCapture::~AudioCapture() = default;

bool AudioCapture::initialize(int sampleRate, int channels, int framesPerBuffer) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    impl_->sampleRate_ = sampleRate;
    impl_->channels_ = channels;
    impl_->framesPerBuffer_ = framesPerBuffer;
    
    return true;
}

void AudioCapture::startCapture(AudioCallback callback) {
    impl_->callback_ = callback;
    
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = impl_->channels_;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;
    
    PaError err = Pa_OpenStream(
        &impl_->stream_,
        &inputParameters,
        nullptr,
        impl_->sampleRate_,
        impl_->framesPerBuffer_,
        paClipOff,
        portAudioCallback,
        this
    );
    
    if (err != paNoError) {
        std::cerr << "Failed to open audio stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
    
    err = Pa_StartStream(impl_->stream_);
    if (err != paNoError) {
        std::cerr << "Failed to start audio stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
    
    impl_->running_ = true;
}

void AudioCapture::stopCapture() {
    if (impl_->running_ && impl_->stream_) {
        PaError err = Pa_StopStream(impl_->stream_);
        if (err != paNoError) {
            std::cerr << "Failed to stop audio stream: " << Pa_GetErrorText(err) << std::endl;
        }
        
        err = Pa_CloseStream(impl_->stream_);
        if (err != paNoError) {
            std::cerr << "Failed to close audio stream: " << Pa_GetErrorText(err) << std::endl;
        }
        
        impl_->stream_ = nullptr;
        impl_->running_ = false;
    }
    
    Pa_Terminate();
}

int AudioCapture::portAudioCallback(const void* input, void* output,
                                  unsigned long frameCount,
                                  const void* timeInfo,
                                  void* statusFlags,
                                  void* userData) {
    auto* capture = static_cast<AudioCapture*>(userData);
    capture->processAudioData(static_cast<const int16_t*>(input), frameCount);
    return paContinue;
}

void AudioCapture::processAudioData(const int16_t* input, unsigned long frameCount) {
    if (impl_->callback_) {
        std::vector<int16_t> audioData(input, input + frameCount * impl_->channels_);
        impl_->callback_(audioData);
    }
}

std::vector<std::string> AudioCapture::getAudioDevices() {
    std::vector<std::string> devices;
    
    Pa_Initialize();
    
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        std::cerr << "Error getting device count: " << Pa_GetErrorText(numDevices) << std::endl;
        Pa_Terminate();
        return devices;
    }
    
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            devices.emplace_back(deviceInfo->name);
        }
    }
    
    Pa_Terminate();
    return devices;
}

int AudioCapture::getSampleRate() const { return impl_->sampleRate_; }
int AudioCapture::getChannels() const { return impl_->channels_; }
bool AudioCapture::isRunning() const { return impl_->running_; }

} // namespace jarvis
