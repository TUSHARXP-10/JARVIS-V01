#include "speech/text_to_speech.h"
#include "utils/logger.h"
#include <iostream>
#include <stdexcept>

#ifdef ESPEAK_FOUND
#include <espeak-ng/speak_lib.h>
#endif

namespace jarvis {

class TextToSpeech::Impl {
public:
    bool initialized = false;
    std::string voice = "default";
    int rate = 175;  // Words per minute
    int volume = 100;  // 0-100
    bool blocking = false;
    
#ifdef ESPEAK_FOUND
    espeak_POSITION_TYPE positionType = POS_CHARACTER;
    espeak_AUDIO_OUTPUT outputType = AUDIO_OUTPUT_SYNCH_PLAYBACK;
#endif
};

TextToSpeech::TextToSpeech() : impl_(std::make_unique<Impl>()) {}
TextToSpeech::~TextToSpeech() {
    cleanup();
}

bool TextToSpeech::initialize() {
#ifdef ESPEAK_FOUND
    // Initialize eSpeak NG
    espeak_Initialize(impl_->outputType, 0, nullptr, 0);
    
    // Set voice properties
    espeak_SetVoiceByName(impl_->voice.c_str());
    espeak_SetParameter(espeakRATE, impl_->rate, 0);
    espeak_SetParameter(espeakVOLUME, impl_->volume, 0);
    
    impl_->initialized = true;
    LOG_INFO("Text-to-speech initialized successfully with eSpeak NG");
    return true;
#else
    LOG_WARNING("eSpeak NG not available - using placeholder implementation");
    impl_->initialized = true;
    return true;
#endif
}

void TextToSpeech::cleanup() {
#ifdef ESPEAK_FOUND
    if (impl_->initialized) {
        espeak_Terminate();
        impl_->initialized = false;
    }
#endif
}

bool TextToSpeech::speak(const std::string& text) {
    if (!impl_->initialized) {
        LOG_ERROR("Text-to-speech not initialized");
        return false;
    }

    if (text.empty()) {
        LOG_WARNING("Empty text provided to speak");
        return false;
    }

#ifdef ESPEAK_FOUND
    unsigned int flags = espeakCHARS_AUTO;
    if (impl_->blocking) {
        flags |= espeakENDPAUSE;
    }
    
    espeak_ERROR result = espeak_Synth(
        text.c_str(),
        text.length() + 1,
        0,  // position
        POS_CHARACTER,
        0,  // end position
        flags,
        nullptr,  // identifier
        nullptr   // user data
    );
    
    if (result != EE_OK) {
        LOG_ERROR("Failed to synthesize speech: " << result);
        return false;
    }
    
    if (impl_->blocking) {
        espeak_Synchronize();
    }
    
    LOG_INFO("Speaking: " << text);
    return true;
#else
    // Placeholder implementation
    std::cout << "[TTS] " << text << std::endl;
    LOG_INFO("Placeholder TTS: " << text);
    return true;
#endif
}

bool TextToSpeech::speakAsync(const std::string& text) {
    // For non-blocking speech, we'll use a separate thread
    std::thread([this, text]() {
        speak(text);
    }).detach();
    return true;
}

void TextToSpeech::setVoice(const std::string& voice) {
    impl_->voice = voice;
#ifdef ESPEAK_FOUND
    if (impl_->initialized) {
        espeak_SetVoiceByName(voice.c_str());
    }
#endif
}

void TextToSpeech::setRate(int wordsPerMinute) {
    impl_->rate = wordsPerMinute;
#ifdef ESPEAK_FOUND
    if (impl_->initialized) {
        espeak_SetParameter(espeakRATE, wordsPerMinute, 0);
    }
#endif
}

void TextToSpeech::setVolume(int volume) {
    impl_->volume = std::max(0, std::min(100, volume));
#ifdef ESPEAK_FOUND
    if (impl_->initialized) {
        espeak_SetParameter(espeakVOLUME, impl_->volume, 0);
    }
#endif
}

void TextToSpeech::setBlocking(bool blocking) {
    impl_->blocking = blocking;
}

std::vector<std::string> TextToSpeech::getAvailableVoices() {
    std::vector<std::string> voices;
    
#ifdef ESPEAK_FOUND
    espeak_VOICE** voice_list = espeak_ListVoices(nullptr);
    if (voice_list) {
        for (int i = 0; voice_list[i] != nullptr; i++) {
            if (voice_list[i]->name) {
                voices.emplace_back(voice_list[i]->name);
            }
        }
    }
#else
    // Placeholder voices
    voices = {"default", "english", "female", "male"};
#endif
    
    return voices;
}

bool TextToSpeech::isInitialized() const {
    return impl_->initialized;
}

int TextToSpeech::getRate() const {
    return impl_->rate;
}

int TextToSpeech::getVolume() const {
    return impl_->volume;
}

std::string TextToSpeech::getVoice() const {
    return impl_->voice;
}

} // namespace jarvis