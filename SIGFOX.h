//  Declare all Arduino-based SIGFOX transceivers.
#ifndef UNABIZ_ARDUINO_SIGFOX_H
#define UNABIZ_ARDUINO_SIGFOX_H

//  According to regulations, messages should be sent only every 10 minutes.
const unsigned long SEND_DELAY = 10 * 60 * 1000;
const unsigned int COMMAND_TIMEOUT = 3000;

//  Newer UnaBiz / Radiocrafts Dev Kit. Default to pin D4 for transmit, pin D5 for receive.
#include "Radiocrafts.h"

//  Older UnaBiz / Akene Dev Kit. Default to pin D4 for receive, pin D5 for transmit.
#include "Akeru.h"

//  Drop all data passed to this port.  Used to suppress echo output.
class NullPort: public Print {
  virtual size_t write(uint8_t) {}
};

#endif  //  UNABIZ_ARDUINO_SIGFOX_H
