# Porcupine Wake Word Engine Setup Guide

This guide will help you set up the Porcupine wake word engine for the Jarvis voice assistant.

## Overview

Porcupine is a highly accurate and efficient wake word engine developed by Picovoice. It allows your application to respond to custom wake words like "Hey Jarvis" or "Computer".

## Prerequisites

1. **Picovoice Account**: Create a free account at [Picovoice Console](https://console.picovoice.ai/)
2. **Access Key**: After creating an account, get your Access Key from the console
3. **CMake 3.20+**: Ensure CMake is installed and in your PATH

## Installation Steps

### 1. Set Environment Variable

Set your Picovoice Access Key as an environment variable:

**Windows (Command Prompt)**:
```cmd
set PICOVOICE_ACCESS_KEY=your_access_key_here
```

**Windows (PowerShell)**:
```powershell
$env:PICOVOICE_ACCESS_KEY="your_access_key_here"
```

**Linux/macOS**:
```bash
export PICOVOICE_ACCESS_KEY=your_access_key_here
```

### 2. Download Porcupine

#### Option A: Manual Download
1. Go to [Picovoice GitHub](https://github.com/Picovoice/porcupine)
2. Download the latest release for your platform
3. Extract to `third_party/porcupine/` in your project directory

#### Option B: Using Git
```bash
git clone https://github.com/Picovoice/porcupine.git third_party/porcupine
```

### 3. Download Wake Word Models

1. Go to [Picovoice Console](https://console.picovoice.ai/)
2. Navigate to "Porcupine Wake Word"
3. Create a new wake word (e.g., "Hey Jarvis")
4. Download the `.ppn` keyword file
5. Download the model file (`.pv` extension)

### 4. Directory Structure

Create the following directory structure:

```
JARVIS.01/
├── third_party/
│   └── porcupine/
│       ├── include/          # Header files
│       ├── lib/             # Library files
│       └── resources/
│           ├── keyword_files/
│           │   └── hey_jarvis_windows.ppn
│           └── porcupine_params.pv
```

### 5. Platform-Specific Libraries

#### Windows
- Download the Windows library from Porcupine releases
- Place `.lib` files in `third_party/porcupine/lib/`
- Place `.dll` files in your executable directory or system PATH

#### Linux
- Download the Linux library
- Place `.so` files in `third_party/porcupine/lib/`

#### macOS
- Download the macOS library
- Place `.dylib` files in `third_party/porcupine/lib/`

## Configuration

Update your `jarvis.json` configuration file:

```json
{
  "wake_word": {
    "enabled": true,
    "model_path": "third_party/porcupine/resources/porcupine_params.pv",
    "keyword_path": "third_party/porcupine/resources/keyword_files/hey_jarvis_windows.ppn",
    "sensitivity": 0.7
  }
}
```

## Testing Wake Word Detection

### 1. Build the Project
```bash
# Windows
.\build.ps1 -BuildType Release

# Linux/macOS
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### 2. Run with Debug Logging
```bash
# Enable debug logging
$env:JARVIS_LOG_LEVEL="DEBUG"
.\jarvis.exe
```

### 3. Test Wake Word
- Say "Hey Jarvis" clearly
- Check console for wake word detection messages
- The system should respond with "Wake word detected!"

## Troubleshooting

### Common Issues

#### "Porcupine not found"
- Ensure the library files are in the correct directory
- Check that `PORCUPINE_INCLUDE_DIR` and `PORCUPINE_LIBRARY` are set correctly
- Verify the CMake configuration output shows "Found Porcupine"

#### "PICOVOICE_ACCESS_KEY not set"
- Make sure the environment variable is set before running the application
- On Windows, set it in System Properties > Environment Variables

#### Audio Issues
- Check that your microphone is working
- Ensure PortAudio is properly installed
- Verify audio permissions on your system

#### Low Detection Accuracy
- Increase sensitivity value (0.0-1.0)
- Ensure you're using the correct keyword file for your platform
- Check audio quality and background noise

### Debug Commands

#### Check Porcupine Detection
```bash
# List audio devices
python -m pip install pyaudio
python -c "import pyaudio; p = pyaudio.PyAudio(); [print(f'{i}: {p.get_device_info_by_index(i)[\"name\"]}') for i in range(p.get_device_count())]"
```

#### Test Audio Input
```bash
# Record and play back audio (Linux/macOS)
arecord -D plughw:1,0 -f S16_LE -r 16000 -c 1 test.wav
aplay test.wav
```

## Custom Wake Words

To create custom wake words:

1. Go to [Picovoice Console](https://console.picovoice.ai/)
2. Create a new wake word
3. Choose your target platform
4. Download the generated `.ppn` file
5. Update the `keyword_path` in your configuration

## Performance Tips

- Use a sensitivity between 0.5-0.8 for best results
- Test in your actual environment with background noise
- Consider using multiple wake words for redundancy
- Monitor CPU usage and adjust frame size if needed

## Additional Resources

- [Porcupine Documentation](https://picovoice.ai/docs/porcupine/)
- [Picovoice GitHub](https://github.com/Picovoice/porcupine)
- [Wake Word Training Guide](https://picovoice.ai/docs/wake-word-training/)
- [Support Forum](https://github.com/Picovoice/porcupine/issues)