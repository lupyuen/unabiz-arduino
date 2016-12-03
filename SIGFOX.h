//  Declare all Arduino-based SIGFOX transceivers.
#ifndef UNABIZ_ARDUINO_SIGFOX_H
#define UNABIZ_ARDUINO_SIGFOX_H

//  According to regulation, messages should be sent only every 10 minutes.
const unsigned long SEND_DELAY = (unsigned long) 10 * 60 * 1000;
const unsigned int MAX_BYTES_PER_MESSAGE = 12;  //  Only 12 bytes per message.
const unsigned int COMMAND_TIMEOUT = 3000;  //  Wait up to 3 seconds for response from SIGFOX module.

//  Define the countries that are supported.
enum Country {
  COUNTRY_AU = 'A'+'U' << 8,  //  Australia
  COUNTRY_BR = 'B'+'R' << 8,  //  Brazil
  COUNTRY_FR = 'F'+'R' << 8,  //  France
  COUNTRY_NZ = 'N'+'Z' << 8,  //  New Zealand
  COUNTRY_SG = 'S'+'G' << 8,  //  Singapore
  COUNTRY_US = 'U'+'S' << 8,  //  USA
  COUNTRY_TW = 'T'+'W' << 8,  //  Taiwan
};

//  Newer UnaBiz / Radiocrafts Dev Kit. Default to pin D4 for transmit, pin D5 for receive.
#include "Radiocrafts.h"

//  Older UnaBiz / Akene Dev Kit. Default to pin D4 for receive, pin D5 for transmit.
#include "Akeru.h"

//  Send structured messages to SIGFOX cloud.
#include "Message.h"

//  Drop all data passed to this port.  Used to suppress echo output.
class NullPort: public Print {
  virtual size_t write(uint8_t) {}
};

//  Call this function if we need to stop.  This informs the emulator to stop listening.
void stop(const String msg);

#endif  //  UNABIZ_ARDUINO_SIGFOX_H
