cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME unabiz-arduino)
project(${PROJECT_NAME})

####
#set(SOURCE_FILES Radiocrafts.cpp)
#add_definitions(-DUNITTEST)
#set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(${CMAKE_PROJECT_NAME}_SRCS
        unabiz-arduino.ino
        Radiocrafts.cpp
        #test/test.cpp
        )
####

set(${CMAKE_PROJECT_NAME}_SKETCH unabiz-arduino.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
