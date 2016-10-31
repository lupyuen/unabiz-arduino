cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME unabiz-arduino)
set(PROJECT_LIB ${PROJECT_NAME}_lib)
project(${PROJECT_NAME})
add_definitions(-DCLION)

# Arduino Options - See https://github.com/queezythegreat/arduino-cmake
# Change this to the Arduino port.
if(APPLE)
    set(${PROJECT_NAME}_PORT zzzz)
elseif(WIN32)
    set(${PROJECT_NAME}_PORT COM1:)
endif()

# Build the library.
set(${PROJECT_LIB}_SRCS Radiocrafts.cpp)
set(${PROJECT_LIB}_HDRS Radiocrafts.h)
generate_arduino_library(${PROJECT_LIB})

# Build the application.
set(${PROJECT_NAME}_SKETCH ${PROJECT_NAME}.ino)
set(${PROJECT_NAME}_ARDLIBS SoftwareSerial)
set(${PROJECT_NAME}_LIBS ${PROJECT_LIB})
generate_arduino_firmware(${PROJECT_NAME})
