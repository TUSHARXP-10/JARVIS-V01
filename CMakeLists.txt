cmake_minimum_required(VERSION 3.20)
project(Jarvis VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Audio dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# JSON library
find_package(nlohmann_json 3.2.0 REQUIRED)

# Boost
find_package(Boost 1.75.0 REQUIRED COMPONENTS system filesystem thread)

# Porcupine wake word engine
find_path(PORCUPINE_INCLUDE_DIR
    NAMES pv_porcupine.h
    PATHS ${CMAKE_SOURCE_DIR}/third_party/porcupine/include
          /usr/local/include/porcupine
          /opt/porcupine/include
          ${CMAKE_CURRENT_SOURCE_DIR}/../porcupine/include
)

find_library(PORCUPINE_LIBRARY
    NAMES pv_porcupine
    PATHS ${CMAKE_SOURCE_DIR}/third_party/porcupine/lib
          /usr/local/lib
          /opt/porcupine/lib
          ${CMAKE_CURRENT_SOURCE_DIR}/../porcupine/lib
)

if(PORCUPINE_INCLUDE_DIR AND PORCUPINE_LIBRARY)
    message(STATUS "Found Porcupine: ${PORCUPINE_LIBRARY}")
    set(PORCUPINE_FOUND TRUE)
else()
    message(WARNING "Porcupine not found - wake word detection will use placeholder implementation")
    set(PORCUPINE_FOUND FALSE)
endif()

# Vosk speech recognition
find_path(VOSK_INCLUDE_DIR
    NAMES vosk_api.h
    PATHS ${CMAKE_SOURCE_DIR}/third_party/vosk/src
          /usr/local/include/vosk
          /opt/vosk/include
)

find_library(VOSK_LIBRARY
    NAMES vosk
    PATHS ${CMAKE_SOURCE_DIR}/third_party/vosk/lib
          /usr/local/lib
          /opt/vosk/lib
          ${CMAKE_CURRENT_SOURCE_DIR}/../vosk/lib
)

if(VOSK_INCLUDE_DIR AND VOSK_LIBRARY)
    message(STATUS "Found Vosk: ${VOSK_LIBRARY}")
    set(VOSK_FOUND TRUE)
else()
    message(WARNING "Vosk not found - speech recognition will use placeholder implementation")
    set(VOSK_FOUND FALSE)
endif()

# eSpeak NG text-to-speech
find_path(ESPEAK_INCLUDE_DIR
    NAMES espeak-ng/speak_lib.h
    PATHS ${CMAKE_SOURCE_DIR}/third_party/espeak-ng/src
          /usr/local/include
          /opt/espeak-ng/include
)

find_library(ESPEAK_LIBRARY
    NAMES espeak-ng espeak
    PATHS ${CMAKE_SOURCE_DIR}/third_party/espeak-ng/lib
          /usr/local/lib
          /opt/espeak-ng/lib
)

if(ESPEAK_INCLUDE_DIR AND ESPEAK_LIBRARY)
    message(STATUS "Found eSpeak NG: ${ESPEAK_LIBRARY}")
    set(ESPEAK_FOUND TRUE)
else()
    message(WARNING "eSpeak NG not found - text-to-speech will use placeholder implementation")
    set(ESPEAK_FOUND FALSE)
endif()

# Threading
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(plugins)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Custom targets
add_custom_target(format
    COMMAND clang-format -i src/**/*.cpp src/**/*.h include/**/*.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting source code"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning build directory"
)

# Installation
install(TARGETS jarvis DESTINATION bin)
install(DIRECTORY configs/ DESTINATION etc/jarvis)
install(DIRECTORY plugins/ DESTINATION lib/jarvis/plugins)