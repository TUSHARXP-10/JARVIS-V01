# Phase 1 Integration Guide: Porcupine, Vosk & eSpeak NG

This guide provides step-by-step instructions for integrating the real libraries in Phase 1 of the JARVIS project.

## Overview
Phase 1 completes the integration of three core speech processing libraries:
- **Porcupine**: Wake word detection
- **Vosk**: Speech-to-text recognition  
- **eSpeak NG**: Text-to-speech synthesis
- **PortAudio**: Audio capture and playback

## Prerequisites

### 1. Environment Setup
- **Windows**: Visual Studio 2019+ or MinGW
- **Linux**: GCC 9+ or Clang
- **macOS**: Xcode 12+ or Homebrew GCC

### 2. Required Libraries

#### Porcupine Wake Word Engine
```bash
# Download from Picovoice Console (https://console.picovoice.ai)
# Place in third_party/porcupine/
# Structure:
# third_party/porcupine/
# ├── include/
# │   └── pv_porcupine.h
# └── lib/
#     ├── libpv_porcupine.a (Linux/macOS)
#     └── pv_porcupine.dll (Windows)
```

#### Vosk Speech Recognition
```bash
# Download from https://alphacephei.com/vosk/
# Structure:
# third_party/vosk/
# ├── src/
# │   └── vosk_api.h
# └── lib/
#     ├── libvosk.so (Linux)
#     ├── libvosk.dylib (macOS)
#     └── vosk.dll (Windows)

# Also download a model:
# third_party/vosk/models/
# └── vosk-model-en-us-0.22/
```

#### eSpeak NG
```bash
# Windows: Install from https://github.com/espeak-ng/espeak-ng/releases
# Linux: sudo apt-get install libespeak-ng-dev
# macOS: brew install espeak-ng

# Structure for custom build:
# third_party/espeak-ng/
# ├── src/
# │   └── speak_lib.h
# └── lib/
#     ├── libespeak-ng.so (Linux)
#     ├── libespeak-ng.dylib (macOS)
#     └── espeak-ng.dll (Windows)
```

## Configuration

### 1. Environment Variables
```bash
# Required for Porcupine
export PICOVOICE_ACCESS_KEY="your_access_key_here"

# Optional library paths
export PORCUPINE_ROOT="/path/to/porcupine"
export VOSK_ROOT="/path/to/vosk"
export ESPEAK_ROOT="/path/to/espeak"
```

### 2. Model Configuration
Create `configs/speech_config.json`:
```json
{
  "wake_word": {
    "model_path": "third_party/porcupine/models/porcupine_params.pv",
    "keyword_path": "third_party/porcupine/keywords/jarvis.ppn",
    "sensitivity": 0.5
  },
  "speech_recognition": {
    "model_path": "third_party/vosk/models/vosk-model-en-us-0.22",
    "sample_rate": 16000
  },
  "text_to_speech": {
    "voice": "en-us",
    "rate": 175,
    "volume": 100
  }
}
```

## Build Instructions

### 1. Configure Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### 2. Build Project
```bash
cmake --build . --config Release
```

### 3. Run Tests
```bash
# Test individual modules
./tests/test_wake_word
./tests/test_speech_recognizer
./tests/test_text_to_speech
./tests/test_audio_capture

# Or run all tests
ctest --verbose
```

## Usage Examples

### 1. Wake Word Detection
```cpp
#include "speech/wake_word_detector.h"

jarvis::WakeWordDetector detector;
detector.initialize("model_path", "keyword_path", 0.5f);
detector.startDetection([]() {
    std::cout << "Wake word detected!" << std::endl;
});
```

### 2. Speech Recognition
```cpp
#include "speech/speech_recognizer.h"

jarvis::SpeechRecognizer recognizer;
recognizer.initialize("model_path", 16000);
std::string result = recognizer.processAudio(audioData);
```

### 3. Text-to-Speech
```cpp
#include "speech/text_to_speech.h"

jarvis::TextToSpeech tts;
tts.initialize();
tts.speak("Hello, how can I help you?");
```

## Troubleshooting

### Common Issues

#### 1. Library Not Found
```bash
# Check library paths
echo $PORCUPINE_ROOT
echo $VOSK_ROOT
echo $ESPEAK_ROOT

# Verify file structure
ls -la third_party/*/lib/
ls -la third_party/*/include/
```

#### 2. Missing Access Key
```bash
# Set Picovoice access key
export PICOVOICE_ACCESS_KEY="your_actual_key"
```

#### 3. Audio Device Issues
```bash
# List audio devices
./tests/test_audio_capture
```

#### 4. Model File Issues
```bash
# Verify model files exist
ls -la third_party/vosk/models/
ls -la third_party/porcupine/models/
```

### Platform-Specific Notes

#### Windows
- Install Visual Studio Build Tools
- Ensure CMake is in PATH
- Use Developer Command Prompt

#### Linux
- Install development packages:
```bash
sudo apt-get install libasound2-dev libpulse-dev
```

#### macOS
- Install Xcode command line tools:
```bash
xcode-select --install
```

## Performance Tuning

### 1. Audio Settings
- Sample rate: 16000 Hz (recommended)
- Frame size: 512 samples
- Channels: 1 (mono)

### 2. Model Selection
- Use smaller models for faster processing
- Choose language-specific models for accuracy

### 3. Threading
- Audio capture runs in dedicated thread
- Processing threads for STT/TTS
- Use thread-safe queues for communication

## Next Steps

After successful Phase 1 integration:
1. **Audio Pipeline**: Connect wake word → STT → NLU → TTS
2. **Plugin System**: Implement dynamic plugin loading
3. **Intent Processing**: Add natural language understanding
4. **Web Interface**: Add HTTP API and web UI

## Verification Checklist

- [ ] Porcupine wake word detection works
- [ ] Vosk speech recognition processes audio
- [ ] eSpeak NG generates speech
- [ ] PortAudio captures microphone input
- [ ] All tests pass
- [ ] Configuration files are valid JSON
- [ ] Environment variables are set
- [ ] Audio devices are accessible
- [ ] Model files are present and valid
- [ ] Build completes without errors