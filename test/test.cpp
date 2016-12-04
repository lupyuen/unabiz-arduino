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

  static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
  static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
  static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
  static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
  static Radiocrafts transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield Dev Kit with Radiocrafts module.

  Message msg(transceiver);
  msg.addField("ctr", 123);
  msg.addField("tmp", 30.1);
  msg.addField("hmd", 98.7);
  String encodedMsg = msg.getEncodedMessage();
  printf("encodedMsg=%s\n", encodedMsg.c_str());
  String decodedMsg = Message::decodeMessage(encodedMsg);
  printf("decodedMsg=%s\n", decodedMsg.c_str());
  msg.send();

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
