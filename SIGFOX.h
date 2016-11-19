//  Declare all Arduino-based SIGFOX transceivers.
#ifndef UNABIZ_ARDUINO_SIGFOX_H
#define UNABIZ_ARDUINO_SIGFOX_H

#include <stdint.h>

//  According to regulation, messages should be sent only every 10 minutes.
const unsigned long SEND_DELAY = (unsigned long) 10 * 60 * 1000;
const unsigned int MAX_BYTES_PER_MESSAGE = 12;  //  Only 12 bytes per message.
const unsigned int COMMAND_TIMEOUT = 3000;  //  Wait up to 3 seconds for response from SIGFOX module.

//  Define the countries that are supported.
enum Country {
  COUNTRY_AU = (int16_t) ((int16_t) 'A') << 8 + 'U',  //  Australia
  COUNTRY_BR = (int16_t) ((int16_t) 'B') << 8 + 'R',  //  Brazil
  COUNTRY_NZ = (int16_t) ((int16_t) 'N') << 8 + 'Z',  //  New Zealand
  COUNTRY_SG = (int16_t) ((int16_t) 'S') << 8 + 'G',  //  Singapore
  COUNTRY_US = (int16_t) ((int16_t) 'U') << 8 + 'S',  //  USA
  COUNTRY_TW = (int16_t) ((int16_t) 'T') << 8 + 'W',  //  Taiwan
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

#endif  //  UNABIZ_ARDUINO_SIGFOX_H
