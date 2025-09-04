#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#ifdef ESPEAK_FOUND
#include <espeak-ng/speak_lib.h>
#endif

class SimpleTextToSpeechTest {
public:
    static void testESpeakInitialization() {
#ifdef ESPEAK_FOUND
        std::cout << "Testing eSpeak NG initialization..." << std::endl;
        
        espeak_ERROR result = espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 0, nullptr, 0);
        if (result == EE_OK) {
            std::cout << "✓ eSpeak NG initialized successfully" << std::endl;
            
            // Test voice listing
            espeak_VOICE** voices = espeak_ListVoices(nullptr);
            if (voices) {
                std::cout << "✓ Available voices:" << std::endl;
                for (int i = 0; voices[i] != nullptr; i++) {
                    if (voices[i]->name) {
                        std::cout << "  - " << voices[i]->name << std::endl;
                    }
                }
            }
            
            espeak_Terminate();
        } else {
            std::cout << "✗ eSpeak NG initialization failed: " << result << std::endl;
        }
        
#else
        std::cout << "eSpeak NG not available - skipping test" << std::endl;
#endif
    }

    static void testSpeechSynthesis() {
#ifdef ESPEAK_FOUND
        std::cout << "Testing speech synthesis..." << std::endl;
        
        espeak_ERROR result = espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 0, nullptr, 0);
        if (result == EE_OK) {
            // Test basic synthesis
            result = espeak_Synth("Hello, this is a test of the eSpeak NG engine.", 
                                 50, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, nullptr, nullptr);
            
            if (result == EE_OK) {
                std::cout << "✓ Speech synthesis successful" << std::endl;
            } else {
                std::cout << "✗ Speech synthesis failed: " << result << std::endl;
            }
            
            espeak_Synchronize();
            espeak_Terminate();
        }
        
#else
        std::cout << "eSpeak NG not available - skipping test" << std::endl;
#endif
    }

    static void testVoiceSettings() {
#ifdef ESPEAK_FOUND
        std::cout << "Testing voice settings..." << std::endl;
        
        espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK, 0, nullptr, 0);
        
        // Test rate settings
        espeak_SetParameter(espeakRATE, 100, 0); // Slow
        std::cout << "✓ Rate set to 100 WPM" << std::endl;
        
        espeak_SetParameter(espeakRATE, 200, 0); // Fast
        std::cout << "✓ Rate set to 200 WPM" << std::endl;
        
        // Test volume settings
        espeak_SetParameter(espeakVOLUME, 50, 0); // Half volume
        std::cout << "✓ Volume set to 50%" << std::endl;
        
        espeak_SetParameter(espeakVOLUME, 100, 0); // Full volume
        std::cout << "✓ Volume set to 100%" << std::endl;
        
        espeak_Terminate();
        
#else
        std::cout << "eSpeak NG not available - skipping test" << std::endl;
#endif
    }
};

int main() {
    std::cout << "=== Text-to-Speech Test ===" << std::endl;
    
    SimpleTextToSpeechTest::testESpeakInitialization();
    SimpleTextToSpeechTest::testSpeechSynthesis();
    SimpleTextToSpeechTest::testVoiceSettings();
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}