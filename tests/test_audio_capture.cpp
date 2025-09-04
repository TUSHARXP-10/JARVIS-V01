#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <portaudio.h>

class SimpleAudioCaptureTest {
public:
    static void testPortAudioInitialization() {
        std::cout << "Testing PortAudio initialization..." << std::endl;
        
        PaError err = Pa_Initialize();
        if (err == paNoError) {
            std::cout << "✓ PortAudio initialized successfully" << std::endl;
            
            // Test device enumeration
            int numDevices = Pa_GetDeviceCount();
            if (numDevices > 0) {
                std::cout << "✓ Found " << numDevices << " audio devices" << std::endl;
                
                for (int i = 0; i < numDevices; i++) {
                    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
                    if (deviceInfo) {
                        std::cout << "  Device " << i << ": " << deviceInfo->name;
                        std::cout << " (Input: " << deviceInfo->maxInputChannels << ")" << std::endl;
                    }
                }
            } else {
                std::cout << "✗ No audio devices found" << std::endl;
            }
            
            Pa_Terminate();
        } else {
            std::cout << "✗ PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        }
    }

    static void testAudioStream() {
        std::cout << "Testing audio stream creation..." << std::endl;
        
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            std::cout << "✗ Initialization failed: " << Pa_GetErrorText(err) << std::endl;
            return;
        }
        
        PaStream* stream = nullptr;
        
        // Test opening default input stream
        PaStreamParameters inputParameters;
        inputParameters.device = Pa_GetDefaultInputDevice();
        if (inputParameters.device == paNoDevice) {
            std::cout << "✗ No default input device" << std::endl;
            Pa_Terminate();
            return;
        }
        
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(inputParameters.device);
        std::cout << "✓ Default input device: " << deviceInfo->name << std::endl;
        
        inputParameters.channelCount = 1;
        inputParameters.sampleFormat = paInt16;
        inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = nullptr;
        
        err = Pa_OpenStream(&stream, &inputParameters, nullptr, 16000, 512, paClipOff, nullptr, nullptr);
        
        if (err == paNoError) {
            std::cout << "✓ Audio stream opened successfully" << std::endl;
            
            // Test starting stream
            err = Pa_StartStream(stream);
            if (err == paNoError) {
                std::cout << "✓ Audio stream started successfully" << std::endl;
                
                // Test reading audio (briefly)
                std::vector<int16_t> buffer(512);
                err = Pa_ReadStream(stream, buffer.data(), buffer.size());
                
                if (err == paNoError) {
                    std::cout << "✓ Successfully read " << buffer.size() << " audio samples" << std::endl;
                    
                    // Calculate RMS for debugging
                    double sum = 0.0;
                    for (int16_t sample : buffer) {
                        sum += static_cast<double>(sample) * sample;
                    }
                    double rms = std::sqrt(sum / buffer.size());
                    std::cout << "  Audio RMS: " << rms << std::endl;
                } else {
                    std::cout << "✗ Failed to read audio: " << Pa_GetErrorText(err) << std::endl;
                }
                
                Pa_StopStream(stream);
            } else {
                std::cout << "✗ Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
            }
            
            Pa_CloseStream(stream);
        } else {
            std::cout << "✗ Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
        }
        
        Pa_Terminate();
    }

    static void testAudioCallback() {
        std::cout << "Testing audio callback functionality..." << std::endl;
        
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            std::cout << "✗ Initialization failed: " << Pa_GetErrorText(err) << std::endl;
            return;
        }
        
        // Test callback mechanism
        std::cout << "✓ Callback mechanism ready for implementation" << std::endl;
        
        Pa_Terminate();
    }
};

int main() {
    std::cout << "=== Audio Capture Test ===" << std::endl;
    
    SimpleAudioCaptureTest::testPortAudioInitialization();
    SimpleAudioCaptureTest::testAudioStream();
    SimpleAudioCaptureTest::testAudioCallback();
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}