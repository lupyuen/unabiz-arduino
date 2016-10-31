cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME unabiz-arduino)
project(${PROJECT_NAME})

####
add_definitions(-DCLION)
set(${CMAKE_PROJECT_NAME}_ARDLIBS SoftwareSerial)

#set(SOURCE_FILES Radiocrafts.cpp)
#set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
#set(${CMAKE_PROJECT_NAME}_SRCS
#        ${PROJECT_NAME}.ino
#        Radiocrafts.cpp
#        )
####

set(${CMAKE_PROJECT_NAME}_SKETCH ${PROJECT_NAME}.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
