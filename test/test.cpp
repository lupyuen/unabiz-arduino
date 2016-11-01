//  Test the transceiver functions under Windows or Mac without Arduino.
#ifndef ARDUINO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include "util.cpp"
#include "../Radiocrafts.cpp"
#include "../unabiz-arduino.ino"

int main() {
  puts("test");
  setup();
  for (;;) {
    loop();
    break;
  }
  return 0;
}
#endif  //  ARDUINO
