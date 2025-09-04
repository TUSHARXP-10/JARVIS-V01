#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>

// Forward declarations for Porcupine
struct pv_porcupine;
typedef struct pv_porcupine pv_porcupine_t;

namespace jarvis {

/**
 * @brief Wake word detection using Porcupine engine
 * 
 * This class provides wake word detection functionality using
 * the Porcupine wake word engine from Picovoice.
 */
class WakeWordDetector {
public:
    using WakeWordCallback = std::function<void()>;

    WakeWordDetector();
    ~WakeWordDetector();

    /**
     * @brief Initialize the wake word detector
     * @param modelPath Path to the Porcupine model file
     * @param keywordPath Path to the keyword file (.ppn)
     * @param sensitivity Sensitivity value [0.0, 1.0]
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& modelPath, 
                   const std::string& keywordPath, 
                   float sensitivity = 0.5f);

    /**
     * @brief Start wake word detection
     * @param callback Function to call when wake word is detected
     */
    void startDetection(WakeWordCallback callback);

    /**
     * @brief Stop wake word detection
     */
    void stopDetection();

    /**
     * @brief Check if detection is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }

    /**
     * @brief Get sample rate required by Porcupine
     * @return Sample rate in Hz
     */
    int getSampleRate() const;

    /**
     * @brief Get frame length required by Porcupine
     * @return Frame length in samples
     */
    int getFrameLength() const;

private:
    std::unique_ptr<pv_porcupine_t, void(*)(pv_porcupine_t*)> porcupine_;
    WakeWordCallback callback_;
    std::thread detectionThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> shouldStop_{false};

    void detectionLoop();
    bool processAudioFrame(const std::vector<int16_t>& frame);
    
    // Audio capture
    class AudioCapture;
    std::unique_ptr<AudioCapture> audioCapture_;
};

} // namespace jarvis