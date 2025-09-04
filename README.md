# 🤖 JARVIS Voice Assistant

A modern, extensible voice assistant built with C++ that provides real-time wake word detection, speech recognition, natural language understanding, and text-to-speech capabilities.

## 🌟 Features

### ✅ **Core Capabilities**

- **Wake Word Detection**: "Hey Jarvis" using Porcupine AI
- **Speech-to-Text**: Real-time transcription with Vosk
- **Natural Language Understanding**: Intent parsing and handling
- **Text-to-Speech**: Natural voice synthesis with eSpeak
- **Plugin System**: Extensible functionality via shared libraries

### 🔧 **Technical Highlights**
- **Lock-free Audio Pipeline**: Real-time processing without audio drops
- **Thread-safe Architecture**: Multi-threaded design for optimal performance
- **Cross-platform**: Windows, Linux, and macOS support
- **Plugin Ecosystem**: Dynamic loading of custom functionality
- **Comprehensive Testing**: Unit tests for all core components

## 📁 Project Structure

```
JARVIS.01/
├── src/                    # Core source code
│   ├── audio/             # Audio pipeline and capture
│   ├── core/              # Main application logic
│   ├── speech/            # Speech processing modules
│   └── utils/             # Utility functions
├── include/               # Header files
├── plugins/               # Plugin system
│   └── sample/            # Sample plugins
├── tests/                 # Unit tests
├── configs/               # Configuration files
└── build/                 # Build artifacts
```

## 🚀 Quick Start

### Prerequisites
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- **CMake** 3.15 or higher
- **Git** for version control

### Required Libraries
- **Porcupine** (wake word detection)
- **Vosk** (speech recognition)
- **eSpeak** (text-to-speech)
- **PortAudio** (audio I/O)

### Installation

#### 1. Clone the Repository
```bash
git clone https://github.com/TUSHARXP-10/JARVIS-V01.git
cd JARVIS-V01
```

#### 2. Install Dependencies

**Windows:**
```powershell
# Install vcpkg packages
vcpkg install portaudio:x64-windows
```

**Linux:**
```bash
# Ubuntu/Debian
sudo apt-get install libportaudio2 libportaudiocpp0 portaudio19-dev libespeak-ng-dev

# CentOS/RHEL
sudo yum install portaudio-devel espeak-ng-devel
```

**macOS:**
```bash
brew install portaudio espeak-ng
```

#### 3. Configure Environment Variables
```bash
# Porcupine (required)
export PICOVOICE_ACCESS_KEY="your_porcupine_access_key"

# Optional custom paths
export VOSK_MODEL_PATH="path/to/vosk/model"
export PORCUPINE_MODEL_PATH="path/to/porcupine/model"
```

#### 4. Build the Project
```bash
# Configure build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)  # Linux/macOS
# or
cmake --build . --config Release  # Windows

# Run tests
ctest --verbose
```

### 5. Run JARVIS
```bash
# Basic usage
./jarvis

# With custom config
./jarvis --config ../configs/jarvis.json

# Enable debug logging
./jarvis --log-level DEBUG
```

## 🎯 Usage Examples

### Basic Voice Commands
- "Hey Jarvis, what time is it?"
- "Hey Jarvis, search for weather"
- "Hey Jarvis, open file documents"

### Plugin Development
Create custom plugins to extend functionality:

```cpp
// my_plugin.cpp
#include "core/plugin.h"

class MyPlugin : public jarvis::IPlugin {
public:
    std::string handleIntent(const jarvis::Intent& intent) override {
        if (intent.name == "my_custom_command") {
            return "Processing your custom command!";
        }
        return "";
    }
    // ... implement other required methods
};
```

## ⚙️ Configuration

### JSON Configuration
Edit `configs/jarvis.json` to customize behavior:

```json
{
  "audio": {
    "sample_rate": 16000,
    "channels": 1,
    "buffer_size": 512
  },
  "wake_word": {
    "sensitivity": 0.5,
    "keyword_path": "models/hey-jarvis.ppn"
  },
  "stt": {
    "model_path": "models/vosk-model-en-us-0.22",
    "timeout_seconds": 15
  },
  "tts": {
    "voice": "en",
    "rate": 175,
    "volume": 100
  }
}
```

## 🧪 Testing

### Run All Tests
```bash
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
ctest --verbose
```

### Individual Component Tests
```bash
# Test audio capture
./tests/test_audio_capture

# Test wake word detection
./tests/test_wake_word

# Test speech recognition
./tests/test_speech_recognizer

# Test text-to-speech
./tests/test_text_to_speech
```

## 📊 Performance

### Benchmarks
- **Wake-to-start latency**: < 200ms
- **End-to-end response**: < 3s for simple commands
- **Memory usage**: ~50MB baseline, ~100MB with models
- **CPU usage**: < 5% idle, < 20% during processing

### Supported Platforms
- **Windows**: 10/11 (x64)
- **Linux**: Ubuntu 18.04+, CentOS 7+
- **macOS**: 10.15+ (x64, Apple Silicon)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Setup
```bash
# Install development dependencies
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Enable debug logging
export JARVIS_LOG_LEVEL=DEBUG

# Run with verbose output
./jarvis --verbose
```

## 📚 Documentation

- **Phase 1 Integration Guide**: `PHASE1_INTEGRATION_GUIDE.md`
- **Phase 2 Audio Pipeline**: `PHASE2_INTEGRATION_GUIDE.md`
- **Porcupine Setup**: `PORCUPINE_SETUP.md`
- **API Documentation**: Generated with Doxygen

## 🔗 Links

- **GitHub Repository**: https://github.com/TUSHARXP-10/JARVIS-V01
- **Issues & Bug Reports**: https://github.com/TUSHARXP-10/JARVIS-V01/issues
- **Discussions**: https://github.com/TUSHARXP-10/JARVIS-V01/discussions

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Porcupine** by Picovoice for wake word detection
- **Vosk** by Alpha Cephei for speech recognition
- **eSpeak** for text-to-speech synthesis
- **PortAudio** for cross-platform audio I/O

## 📞 Support

For questions, issues, or contributions:
- **GitHub Issues**: Report bugs and feature requests
- **Discussions**: General questions and community support
- **Email**: tusharxp10@gmail.com

---

**Built with ❤️ for the voice assistant community**