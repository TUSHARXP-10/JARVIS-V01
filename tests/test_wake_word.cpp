#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>

#ifdef PORCUPINE_FOUND
#include <pv_porcupine.h>
#endif

class SimpleWakeWordTest {
public:
    static void testPorcupineInitialization() {
#ifdef PORCUPINE_FOUND
        std::cout << "Testing Porcupine initialization..." << std::endl;
        
        const char* access_key = std::getenv("PICOVOICE_ACCESS_KEY");
        if (!access_key) {
            std::cerr << "ERROR: PICOVOICE_ACCESS_KEY not set" << std::endl;
            return;
        }

        pv_porcupine_t* porcupine = nullptr;
        
        // Test with invalid paths (should fail)
        pv_status_t status = pv_porcupine_init(
            access_key,
            "invalid_model_path.ppn",
            1,
            nullptr,
            nullptr,
            &porcupine
        );
        
        if (status != PV_STATUS_SUCCESS) {
            std::cout << "✓ Invalid model path correctly rejected" << std::endl;
        } else {
            std::cout << "✗ Invalid model path was accepted" << std::endl;
            pv_porcupine_delete(porcupine);
        }
        
        std::cout << "Porcupine version: " << pv_porcupine_version() << std::endl;
        std::cout << "Sample rate: " << pv_sample_rate() << std::endl;
        std::cout << "Frame length: " << pv_porcupine_frame_length() << std::endl;
        
#else
        std::cout << "Porcupine not available - skipping test" << std::endl;
#endif
    }

    static void testFrameProcessing() {
#ifdef PORCUPINE_FOUND
        std::cout << "Testing frame processing..." << std::endl;
        
        const char* access_key = std::getenv("PICOVOICE_ACCESS_KEY");
        if (!access_key) {
            std::cerr << "ERROR: PICOVOICE_ACCESS_KEY not set" << std::endl;
            return;
        }

        // This test requires actual model files - skipping for now
        std::cout << "Frame processing test requires model files" << std::endl;
        
#else
        std::cout << "Porcupine not available - skipping test" << std::endl;
#endif
    }
};

int main() {
    std::cout << "=== Wake Word Detector Test ===" << std::endl;
    
    SimpleWakeWordTest::testPorcupineInitialization();
    SimpleWakeWordTest::testFrameProcessing();
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}