cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME unabiz-arduino)
project(${PROJECT_NAME})

####  Custom Options - See https://github.com/queezythegreat/arduino-cmake
# Change this to the Arduino port.
if(APPLE)
    set(${CMAKE_PROJECT_NAME}_PORT zzzz)
elseif(WIN32)
    set(${CMAKE_PROJECT_NAME}_PORT COM1:)
endif()
add_definitions(-DCLION)
set(${CMAKE_PROJECT_NAME}_ARDLIBS SoftwareSerial)
set(SoftwareSerial_RECURSE True)

set(${CMAKE_PROJECT_NAME}_SKETCH ${PROJECT_NAME}.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
