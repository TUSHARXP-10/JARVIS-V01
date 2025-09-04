set(SOURCES
    main.cpp
    core/jarvis_core.cpp
    audio/audio_capture.cpp
    audio/audio_player.cpp
    speech/wake_word_detector.cpp
    speech/speech_recognizer.cpp
    speech/text_to_speech.cpp
    nlu/intent_parser.cpp
    nlu/intent_registry.cpp
    plugins/plugin_manager.cpp
    utils/config_manager.cpp
    utils/logger.cpp
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/include/core/jarvis_core.h
    ${CMAKE_SOURCE_DIR}/include/audio/audio_capture.h
    ${CMAKE_SOURCE_DIR}/include/audio/audio_player.h
    ${CMAKE_SOURCE_DIR}/include/speech/wake_word_detector.h
    ${CMAKE_SOURCE_DIR}/include/speech/speech_recognizer.h
    ${CMAKE_SOURCE_DIR}/include/speech/text_to_speech.h
    ${CMAKE_SOURCE_DIR}/include/nlu/intent_parser.h
    ${CMAKE_SOURCE_DIR}/include/nlu/intent_registry.h
    ${CMAKE_SOURCE_DIR}/include/plugins/plugin_manager.h
    ${CMAKE_SOURCE_DIR}/include/utils/config_manager.h
    ${CMAKE_SOURCE_DIR}/include/utils/logger.h
)

# Create the main executable
add_executable(jarvis ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(jarvis 
    ${PORTAUDIO_LIBRARIES}
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::filesystem
    Boost::thread
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

# Porcupine library (if found)
if(PORCUPINE_FOUND)
    target_link_libraries(jarvis ${PORCUPINE_LIBRARY})
    target_compile_definitions(jarvis PRIVATE PORCUPINE_FOUND=1)
    target_include_directories(jarvis PRIVATE ${PORCUPINE_INCLUDE_DIR})
endif()

# Vosk library (if found)
if(VOSK_FOUND)
    target_link_libraries(jarvis ${VOSK_LIBRARY})
    target_compile_definitions(jarvis PRIVATE VOSK_FOUND=1)
    target_include_directories(jarvis PRIVATE ${VOSK_INCLUDE_DIR})
endif()

# eSpeak NG library (if found)
if(ESPEAK_FOUND)
    target_link_libraries(jarvis ${ESPEAK_LIBRARY})
    target_compile_definitions(jarvis PRIVATE ESPEAK_FOUND=1)
    target_include_directories(jarvis PRIVATE ${ESPEAK_INCLUDE_DIR})
endif()

# Include directories for the executable
target_include_directories(jarvis PRIVATE ${PORTAUDIO_INCLUDE_DIRS})

# Platform-specific libraries
if(WIN32)
    target_link_libraries(jarvis winmm)
elseif(UNIX)
    target_link_libraries(jarvis pthread)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(jarvis PRIVATE -Wall -Wextra -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(jarvis PRIVATE -Wall -Wextra -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(jarvis PRIVATE /W4 /O2)
endif()

# Enable debug symbols in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(jarvis PRIVATE -g)
endif()