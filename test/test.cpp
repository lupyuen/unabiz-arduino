//  Test the transceiver functions under Windows or Mac without Arduino.
#ifndef ARDUINO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include "util.cpp"
#include "../Radiocrafts.cpp"
#include "../Akeru.cpp"
#include "../Message.cpp"

int main() {
  puts("test");
  Radiocrafts transceiver;
  Message msg(transceiver);
  msg.addField("tmp", 30.1);
  msg.addField("hmd", 98.7);
  String encodedMsg = msg.getEncodedMessage();
  printf("encodedMsg=%s\n", encodedMsg.c_str());
  String decodedMsg = Message::decodeMessage(encodedMsg);
  printf("decodedMsg=%s\n", decodedMsg.c_str());

#if NOTUSED
  setup();
  for (;;) {
    loop();
    break;
  }
#endif
  return 0;
}
#endif  //  ARDUINO
