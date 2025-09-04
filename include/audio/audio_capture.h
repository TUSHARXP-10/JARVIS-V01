#pragma once

#include <functional>
#include <vector>
#include <memory>

// Forward declaration for PortAudio
typedef struct PaStream PaStream;

namespace jarvis {

/**
 * @brief Audio capture using PortAudio
 * 
 * This class provides cross-platform audio capture functionality
 * using the PortAudio library.
 */
class AudioCapture {
public:
    using AudioCallback = std::function<void(const std::vector<int16_t>&)>;

    AudioCapture();
    ~AudioCapture();

    /**
     * @brief Initialize audio capture
     * @param sampleRate Sample rate in Hz (default: 16000)
     * @param channels Number of channels (default: 1 for mono)
     * @param framesPerBuffer Buffer size (default: 1024)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(int sampleRate = 16000, 
                   int channels = 1, 
                   int framesPerBuffer = 1024);

    /**
     * @brief Start audio capture
     * @param callback Function to call with audio data
     */
    void startCapture(AudioCallback callback);

    /**
     * @brief Stop audio capture
     */
    void stopCapture();

    /**
     * @brief Check if capture is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }

    /**
     * @brief Get sample rate
     * @return Current sample rate in Hz
     */
    int getSampleRate() const { return sampleRate_; }

    /**
     * @brief Get number of channels
     * @return Number of channels
     */
    int getChannels() const { return channels_; }

    /**
     * @brief Get available audio devices
     * @return Vector of device names
     */
    static std::vector<std::string> getAudioDevices();

private:
    PaStream* stream_;
    AudioCallback callback_;
    bool running_;
    int sampleRate_;
    int channels_;
    int framesPerBuffer_;

    static int portAudioCallback(const void* input,
                                void* output,
                                unsigned long frameCount,
                                const void* timeInfo,
                                void* statusFlags,
                                void* userData);

    void processAudioData(const int16_t* input, unsigned long frameCount);
};

} // namespace jarvis