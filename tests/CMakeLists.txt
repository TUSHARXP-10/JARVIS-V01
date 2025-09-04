# Test executables
add_executable(test_wake_word test_wake_word.cpp)
add_executable(test_speech_recognizer test_speech_recognizer.cpp)
add_executable(test_text_to_speech test_text_to_speech.cpp)
add_executable(test_audio_capture test_audio_capture.cpp)

# Link libraries for tests
target_link_libraries(test_wake_word 
    ${PORCUPINE_LIBRARY}
    Threads::Threads
)

if(PORCUPINE_FOUND)
    target_compile_definitions(test_wake_word PRIVATE PORCUPINE_FOUND=1)
    target_include_directories(test_wake_word PRIVATE ${PORCUPINE_INCLUDE_DIR})
endif()

target_link_libraries(test_speech_recognizer 
    ${VOSK_LIBRARY}
    Threads::Threads
)

if(VOSK_FOUND)
    target_compile_definitions(test_speech_recognizer PRIVATE VOSK_FOUND=1)
    target_include_directories(test_speech_recognizer PRIVATE ${VOSK_INCLUDE_DIR})
endif()

target_link_libraries(test_text_to_speech 
    ${ESPEAK_LIBRARY}
    Threads::Threads
)

if(ESPEAK_FOUND)
    target_compile_definitions(test_text_to_speech PRIVATE ESPEAK_FOUND=1)
    target_include_directories(test_text_to_speech PRIVATE ${ESPEAK_INCLUDE_DIR})
endif()

target_link_libraries(test_audio_capture 
    ${PORTAUDIO_LIBRARIES}
    Threads::Threads
)

target_include_directories(test_audio_capture PRIVATE ${PORTAUDIO_INCLUDE_DIRS})